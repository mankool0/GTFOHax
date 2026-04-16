#include "enemy.h"
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <format>
#include <helpers.h>
#include "aimbot.h"
#include "esp.h"
#include "utils/math.h"

namespace Enemy
{
    std::atomic<std::shared_ptr<EnemyVec>> enemies;
    std::atomic<std::shared_ptr<EnemyVec>> enemiesAimbot;
    std::map<std::string, int> enemyIDs;
    std::vector<std::string> enemyNames;

    std::map<app::EnemyAgent*, EnemyPositionHistory> enemyPositionHistory;
    std::mutex enemyPositionHistoryMtx;

    struct BoneLineCastCache
    {
        app::Vector3 lastBonePos      = {0.0f, 0.0f, 0.0f};
        app::Vector3 lastPlayerEyePos = {0.0f, 0.0f, 0.0f};
        bool         lastVisible      = false;
        bool         valid            = false;
    };

    struct EnemyCacheEntry
    {
        BoneLineCastCache boneCache[64];
        app::Transform*   boneTransforms[64];
        bool              boneTransformCached[64];
        BoneLineCastCache fallbackCache;
        std::string       enemyName;

        EnemyCacheEntry()
        {
            memset(boneTransforms, 0, sizeof(boneTransforms));
            memset(boneTransformCached, 0, sizeof(boneTransformCached));
        }
    };

    static std::unordered_map<app::EnemyAgent*, EnemyCacheEntry> linecastCache;
    static constexpr float BONE_MOVE_THRESHOLD_SQ = 0.01f * 0.01f; // 1cm
    static constexpr float EYE_MOVE_THRESHOLD_SQ  = 0.01f * 0.01f; // 1cm

    static constexpr int PERF_LOG_INTERVAL = 120;

    struct PerfStats
    {
        int    frames      = 0;
        double totalUs     = 0.0;
        double minUs       = 1e9;
        double maxUs       = 0.0;
        int    raycasts    = 0;
        int    cacheHits   = 0;
        int    animCalls   = 0;

        void record(double frameUs, int frameCasts, int frameHits, int frameAnimCalls)
        {
            ++frames;
            totalUs    += frameUs;
            if (frameUs < minUs) minUs = frameUs;
            if (frameUs > maxUs) maxUs = frameUs;
            raycasts   += frameCasts;
            cacheHits  += frameHits;
            animCalls  += frameAnimCalls;
        }

        void logAndReset()
        {
            if (frames == 0) return;
            double avgUs    = totalUs / frames;
            int    totalOps = raycasts + cacheHits;
            float  hitRate  = totalOps > 0 ? 100.f * cacheHits / totalOps : 0.f;
            il2cppi_log_write(std::format(
                "[EnemyPerf] frames={} avg={:.1f}us min={:.1f}us max={:.1f}us | "
                "animCalls={} raycasts={} cacheHits={} hitRate={:.1f}%",
                frames, avgUs, minUs, maxUs,
                animCalls, raycasts, cacheHits, hitRate));
            *this = PerfStats{};
        }
    };
    static PerfStats perf;

    app::Vector3 GetEnemyMovementDirection(app::EnemyAgent* enemy)
    {
        app::Vector3 zeroVec = {0.0f, 0.0f, 0.0f};
        if (enemy == nullptr) return zeroVec;
        std::lock_guard<std::mutex> lock(enemyPositionHistoryMtx);
        auto it = enemyPositionHistory.find(enemy);
        if (it != enemyPositionHistory.end() && it->second.hasValidDirection)
            return it->second.movementDirection;
        return zeroVec;
    }

    static int s_frameCasts     = 0;
    static int s_frameHits      = 0;
    static int s_frameAnimCalls = 0;

    static bool isBoneVisible_cached(BoneLineCastCache& cache, const app::Vector3& bonePos,
                                     const app::Vector3& eyePos)
    {
        if (cache.valid)
        {
            float bdx = bonePos.x - cache.lastBonePos.x,
                  bdy = bonePos.y - cache.lastBonePos.y,
                  bdz = bonePos.z - cache.lastBonePos.z;
            float edx = eyePos.x - cache.lastPlayerEyePos.x,
                  edy = eyePos.y - cache.lastPlayerEyePos.y,
                  edz = eyePos.z - cache.lastPlayerEyePos.z;
            if (bdx*bdx + bdy*bdy + bdz*bdz < BONE_MOVE_THRESHOLD_SQ &&
                edx*edx + edy*edy + edz*edz < EYE_MOVE_THRESHOLD_SQ)
            {
                ++s_frameHits;
                return cache.lastVisible;
            }
        }
        
        ++s_frameCasts;
        cache.lastVisible = !app::Physics_Linecast_1(
            eyePos, bonePos,
            (*app::LayerManager__TypeInfo)->static_fields->MASK_DEFAULT, NULL);
        cache.lastBonePos      = bonePos;
        cache.lastPlayerEyePos = eyePos;
        cache.valid            = true;
        return cache.lastVisible;
    }

    bool isValidDistance(bool visible, float distance)
    {
        if (visible)
        {
            if (ESP::enemyESP.visibleSec.show && distance < ESP::enemyESP.visibleSec.renderDistance)
                return true;
            if (Aimbot::settings.toggleKey.isToggled() && distance < Aimbot::settings.maxDistance)
                return true;
        }
        else
        {
            if (ESP::enemyESP.nonVisibleSec.show && distance < ESP::enemyESP.nonVisibleSec.renderDistance)
                return true;
            if (Aimbot::settings.toggleKey.isToggled() && !Aimbot::settings.visibleOnly && distance < Aimbot::settings.maxDistance)
                return true;
        }
        return false;
    }

    void _RefreshEnemyAgents()
    {
        if (!ESP::enemyESP.toggleKey.isToggled() && !Aimbot::settings.toggleKey.isToggled())
            return;
        if (G::localPlayer == nullptr)
            return;

        auto perfStart  = std::chrono::high_resolution_clock::now();
        s_frameCasts    = 0;
        s_frameHits      = 0;
        s_frameAnimCalls = 0;

        app::Vector3 eyePos = G::localPlayer->fields.m_eyePosition;
        app::Vector3 localPos = G::localPlayer->fields.m_goodPosition;

        const bool needVisibilityCheck =
            ESP::enemyESP.visibleSec.show || ESP::enemyESP.nonVisibleSec.show ||
            (Aimbot::settings.toggleKey.isToggled() && Aimbot::settings.visibleOnly);

        static EnemyVec enemiesTemp;
        enemiesTemp.clear();
        if (enemiesTemp.capacity() < 64) enemiesTemp.reserve(128);

        auto courseNodesList = (*app::StaticUpdateManager__TypeInfo)->static_fields->courseNodes;
        for (int i = 0; i < courseNodesList->fields._size; i++)
        {
            auto courseNode = courseNodesList->fields._items->vector[i];
            if (!courseNode) continue;
            auto enemiesList = courseNode->fields.m_enemiesInNode;
            if (!enemiesList) continue;

            for (int j = 0; j < enemiesList->fields._size; j++)
            {
                auto enemyAgent = enemiesList->fields._items->vector[j];
                if (enemyAgent == NULL || !enemyAgent->fields.m_alive)
                    continue;

                app::Vector3 enemyPos = app::EnemyAgent_get_Position(enemyAgent, NULL);
                float dx = enemyPos.x - localPos.x, dy = enemyPos.y - localPos.y, dz = enemyPos.z - localPos.z;
                float distanceSq = dx*dx + dy*dy + dz*dz;
                float distance = sqrtf(distanceSq);

                float maxDist = (std::max<float>)((std::max<float>)((float)ESP::enemyESP.visibleSec.renderDistance, (float)ESP::enemyESP.nonVisibleSec.renderDistance), (float)Aimbot::settings.maxDistance);
                if (distance > maxDist)
                {
                    if (distance > maxDist + 50.0f) linecastCache.erase(enemyAgent);
                    continue;
                }

                auto& cacheEntry = linecastCache[enemyAgent];
                if (cacheEntry.enemyName.empty())
                    cacheEntry.enemyName = il2cppi_to_string(
                        app::Object_1_GetName(reinterpret_cast<app::Object_1*>(enemyAgent), NULL));

                struct TempLimb { app::Vector3 pos; app::Dam_EnemyDamageLimb* ptr; };
                TempLimb tempLimbs[32];
                int tempLimbCount = 0;

                auto enemyInfo = std::make_shared<EnemyInfo>();
                enemyInfo->visible = false;
                enemyInfo->enemyAgent = enemyAgent;
                enemyInfo->enemyObjectName = cacheEntry.enemyName;
                enemyInfo->distance = distance;

                auto damageLimbsList = enemyAgent->fields.Damage->fields.DamageLimbs;
                if (damageLimbsList) {
                    auto damageLimbs = damageLimbsList->vector;
                    int limbCount = (std::min)((int)damageLimbsList->max_length, 32);
                    for (int k = 0; k < limbCount; k++)
                    {
                        auto limb = damageLimbs[k];
                        if (!limb) continue;
                        auto limbPos = app::Dam_EnemyDamageLimb_get_DamageTargetPos(limb, NULL);
                        tempLimbs[tempLimbCount++] = { limbPos, limb };
                        Bone& bone = enemyInfo->damageableBones[enemyInfo->damageableBoneCount++];
                        bone.position = limbPos;
                        bone.damageable = true;
                        bone.destroyed = limb->fields._IsDestroyed_k__BackingField;
                        bone.limbType = limb->fields.m_type;
                        bone.health = limb->fields.m_health;
                        bone.limbPtr = limb;
                    }
                }

                for (auto boneType : Enemy::WantedBones)
                {
                    int idx = static_cast<int>(boneType);
                    if (idx < 0 || idx >= 64) continue;

                    app::Transform* boneTransform;
                    if (cacheEntry.boneTransformCached[idx])
                    {
                        boneTransform = cacheEntry.boneTransforms[idx];
                    }
                    else
                    {
                        ++s_frameAnimCalls;
                        boneTransform = app::Animator_GetBoneTransform(enemyAgent->fields.Anim, boneType, NULL);
                        cacheEntry.boneTransforms[idx] = boneTransform;
                        cacheEntry.boneTransformCached[idx] = true;
                    }
                    if (boneTransform == nullptr) continue;

                    Bone bone;
                    app::Transform_get_position_Injected(boneTransform, &bone.position, NULL);

                    if (needVisibilityCheck)
                    {
                        auto& boneCache = cacheEntry.boneCache[idx];
                        if (isBoneVisible_cached(boneCache, bone.position, eyePos))
                            enemyInfo->visible = true;
                        bone.visible = boneCache.lastVisible;
                    }

                    for (int k = 0; k < tempLimbCount; k++) {
                        if (tempLimbs[k].pos.x == bone.position.x &&
                            tempLimbs[k].pos.y == bone.position.y &&
                            tempLimbs[k].pos.z == bone.position.z) {
                            auto db = tempLimbs[k].ptr;
                            bone.damageable = true;
                            bone.destroyed = db->fields._IsDestroyed_k__BackingField;
                            bone.limbType = db->fields.m_type;
                            bone.health = db->fields.m_health;
                            bone.limbPtr = db;
                            break;
                        }
                    }

                    enemyInfo->bones[idx] = std::move(bone);
                    enemyInfo->hasBone[idx] = true;
                }

                bool hasEssentialBones = enemyInfo->hasBone[static_cast<int>(app::HumanBodyBones__Enum::Head)] &&
                                         enemyInfo->hasBone[static_cast<int>(app::HumanBodyBones__Enum::LeftFoot)] &&
                                         enemyInfo->hasBone[static_cast<int>(app::HumanBodyBones__Enum::RightFoot)];

                if (!hasEssentialBones)
                {
                    enemyInfo->fallbackBone.position = enemyPos;
                    if (needVisibilityCheck)
                    {
                        isBoneVisible_cached(cacheEntry.fallbackCache, enemyInfo->fallbackBone.position, eyePos);
                        enemyInfo->fallbackBone.visible = cacheEntry.fallbackCache.lastVisible;
                        enemyInfo->visible = enemyInfo->fallbackBone.visible;
                    }
                    enemyInfo->useFallback = true;
                }

                if (isValidDistance(enemyInfo->visible, distance))
                    enemiesTemp.push_back(std::move(enemyInfo));
            }
        }

        std::unordered_set<app::EnemyAgent*> validEnemies;
        {
            std::lock_guard<std::mutex> lock(enemyPositionHistoryMtx);
            for (const auto& enemyInfo : enemiesTemp)
            {
                app::EnemyAgent* agent = enemyInfo->enemyAgent;
                validEnemies.insert(agent);
                app::Vector3 currentPos = app::EnemyAgent_get_Position(agent, NULL);
                auto it = enemyPositionHistory.find(agent);
                if (it != enemyPositionHistory.end())
                {
                    it->second.previousPosition = it->second.currentPosition;
                    it->second.currentPosition  = currentPos;
                    float dx = currentPos.x - it->second.previousPosition.x;
                    float dz = currentPos.z - it->second.previousPosition.z;
                    float lenSq = dx*dx + dz*dz;
                    if (lenSq > 0.0001f)
                    {
                        float len = sqrtf(lenSq);
                        it->second.movementDirection = { dx / len, 0.0f, dz / len };
                        it->second.hasValidDirection = true;
                    }
                }
                else
                    enemyPositionHistory[agent] = { currentPos, currentPos, {0,0,0}, false };
            }
            for (auto it = enemyPositionHistory.begin(); it != enemyPositionHistory.end(); )
            {
                if (validEnemies.find(it->first) == validEnemies.end())
                    it = enemyPositionHistory.erase(it);
                else
                    ++it;
            }
        }

        for (auto it = linecastCache.begin(); it != linecastCache.end(); )
            it = validEnemies.count(it->first) ? std::next(it) : linecastCache.erase(it);

        auto sharedVec = std::make_shared<EnemyVec>(std::move(enemiesTemp));
        enemies.store(sharedVec);
        enemiesAimbot.store(std::move(sharedVec));

        auto perfEnd = std::chrono::high_resolution_clock::now();
        double frameUs = std::chrono::duration<double, std::micro>(perfEnd - perfStart).count();
        perf.record(frameUs, s_frameCasts, s_frameHits, s_frameAnimCalls);
        if (perf.frames >= PERF_LOG_INTERVAL)
            perf.logAndReset();
    }

    void _SpawnEnemy(int id, app::AgentMode__Enum agentMode)
    {
        app::EnemyAllocator* enemyAllocator = (*app::EnemyAllocator__TypeInfo)->static_fields->Current;
        app::PlayerAgent* localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        if (!localPlayer) return;
        app::Agent* localPlayerAgent = reinterpret_cast<app::Agent*>(localPlayer);
        app::Quaternion playerRotation = app::Agent_get_Rotation(localPlayerAgent, NULL);
        app::AIG_CourseNode* courseNode = localPlayer->fields.m_courseNode;

        app::Vector3 screenCenter = { G::screenWidth / 2.0f, G::screenHeight / 2.0f, 0 };
        app::Ray screenCenterRay = app::Camera_ScreenPointToRay_2(G::mainCamera, screenCenter, NULL);
        app::RaycastHit raycastHit;
        if (app::Physics_Raycast_14(screenCenterRay, &raycastHit, 200, NULL))
        {
            app::EnemyAllocator_ResetAllowedToSpawn(NULL);
            app::EnemyAllocator_SpawnEnemy(enemyAllocator, id, courseNode, agentMode, raycastHit.m_Point, playerRotation, nullptr, 0, nullptr);
        }
    }

    void RefreshEnemyAgents()
    {
        G::callbacks.push([] { _RefreshEnemyAgents(); });
    }

    void SpawnEnemy(int id, app::AgentMode__Enum agentMode)
    {
        G::callbacks.push([id, agentMode] { _SpawnEnemy(id, agentMode); });
    }
}
