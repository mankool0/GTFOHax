#include "enemy.h"
#include <iostream>
#include <helpers.h>
#include "aimbot.h"
#include "esp.h"
#include "../math.h"

namespace Enemy
{
    std::vector<std::shared_ptr<EnemyInfo>> enemies;
    std::vector<std::shared_ptr<EnemyInfo>> enemiesAimbot;
    std::map<std::string, int> enemyIDs;
    std::vector<std::string> enemyNames;
    
    bool isBoneVisible(Enemy::Bone& bone)
    {
        bone.visible = !app::Physics_Linecast_1(G::localPlayer->fields.m_eyePosition, bone.position, (*app::LayerManager__TypeInfo)->static_fields->MASK_DEFAULT, NULL);
        return bone.visible;
    }

    bool isValidDistance(bool visible, float distance)
    {
        if (visible)
        {
            if (ESP::enemyESP.visibleSec.show && distance < ESP::enemyESP.visibleSec.renderDistance)
                return true;
            if (Aimbot::settings.toggleKey.isToggled() && distance < Aimbot::settings.maxDistance)
                return true;
            return false;
        }
        else {
            if (ESP::enemyESP.nonVisibleSec.show && distance < ESP::enemyESP.nonVisibleSec.renderDistance)
                return true;
            if (Aimbot::settings.toggleKey.isToggled() && !Aimbot::settings.visibleOnly && distance < Aimbot::settings.maxDistance)
                return true;
            return false;
        }
    }

    void _RefreshEnemyAgents()
    {
        if (!ESP::enemyESP.toggleKey.isToggled() && !Aimbot::settings.toggleKey.isToggled())
            return;
        if (G::localPlayer == nullptr)
            return;

        std::vector<std::shared_ptr<EnemyInfo>> enemiesTemp;

        auto courseNodesList = (*app::StaticUpdateManager__TypeInfo)->static_fields->courseNodes;
        for (int i = 0; i < courseNodesList->fields._size; i++)
        {
            auto courseNode = courseNodesList->fields._items->vector[i];
            auto enemiesList = courseNode->fields.m_enemiesInNode;
            for (int j = 0; j < enemiesList->fields._size; j++)
            {
                auto enemyAgent = enemiesList->fields._items->vector[j];
                if (enemyAgent == NULL)
                {
                    //std::cout << "found null enemyAgent\n";
                    continue;
                }

                float distance = app::Vector3_Distance(app::EnemyAgent_get_Position(enemyAgent, NULL), G::localPlayer->fields.m_goodPosition, NULL);
                if ((!Aimbot::settings.toggleKey.isToggled() || distance > Aimbot::settings.maxDistance)
                    && (!ESP::enemyESP.visibleSec.show || distance > ESP::enemyESP.visibleSec.renderDistance)
                    && (!ESP::enemyESP.nonVisibleSec.show || distance > ESP::enemyESP.nonVisibleSec.renderDistance))
                {
                    continue; // No point of keeping around info of enemies we don't use
                }

                app::Object_1* enemyObject = reinterpret_cast<app::Object_1*>(enemyAgent);
                std::string enemyName = il2cppi_to_string(app::Object_1_GetName(enemyObject, NULL));

                using h = std::hash<float>;
                static auto hash = [](const app::Vector3& v) { return h()(v.x) + h()(v.y) + h()(v.z); };
                static auto equal = [](const app::Vector3& lhs, const app::Vector3& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; };
                std::unordered_map<app::Vector3, app::Dam_EnemyDamageLimb*, decltype(hash), decltype(equal)> damageableBones(8, hash, equal);
                std::vector<Bone> damageableBonesVec;

                auto damageLimbsList = enemyAgent->fields.Damage->fields.DamageLimbs;
                auto damageLimbs = damageLimbsList->vector;

                for (int i = 0; i < damageLimbsList->max_length; i++)
                {
                    auto limb = damageLimbs[i];
                    auto limbPos = app::Dam_EnemyDamageLimb_get_DamageTargetPos(limb, NULL);
                    damageableBones.insert(std::pair<app::Vector3, app::Dam_EnemyDamageLimb*>(limbPos, limb));

                    Bone bone;
                    bone.position = limbPos;
                    isBoneVisible(bone);
                    bone.damageable = true;
                    bone.destroyed = limb->fields._IsDestroyed_k__BackingField;
                    bone.limbType = limb->fields.m_type;
                    bone.health = limb->fields.m_health;
                    damageableBonesVec.push_back(bone);
                }

                std::map<app::HumanBodyBones__Enum, Bone> bonesMap;
                bool visible = false;
                for (std::vector<app::HumanBodyBones__Enum>::const_iterator it = Enemy::WantedBones.begin(); it != Enemy::WantedBones.end(); ++it)
                {
                    auto boneTransform = app::Animator_GetBoneTransform(enemyAgent->fields.Anim, (*it), NULL);
                    if (boneTransform == NULL)
                        continue;
                    Bone bone;
                    bone.position = app::Transform_get_position(boneTransform, NULL);
                    if (isBoneVisible(bone))
                        visible = true;
                    if (damageableBones.contains(bone.position))
                    {
                        auto damageableBone = damageableBones[bone.position];
                        bone.damageable = true;
                        bone.destroyed = damageableBone->fields._IsDestroyed_k__BackingField;
                        bone.limbType = damageableBone->fields.m_type;
                        bone.health = damageableBone->fields.m_health;
                    }
                    bonesMap.insert(std::pair<app::HumanBodyBones__Enum, Bone>((*it), bone));
                }
                if (!bonesMap.contains(app::HumanBodyBones__Enum::Head) ||
                    !bonesMap.contains(app::HumanBodyBones__Enum::LeftFoot) ||
                    !bonesMap.contains(app::HumanBodyBones__Enum::RightFoot))
                {
                    Bone fallbackBone;
                    fallbackBone.position = app::EnemyAgent_get_Position(enemyAgent, NULL);
                    if (isBoneVisible(fallbackBone))
                        visible = true;

                    float distance = app::Vector3_Distance(fallbackBone.position, G::localPlayer->fields.m_goodPosition, NULL);
                    if (!isValidDistance(visible, distance))
                        continue;
                    auto enInfo = std::shared_ptr<EnemyInfo>(new EnemyInfo(visible, bonesMap, damageableBonesVec, enemyAgent, enemyName, distance, fallbackBone));
                    enemiesTemp.push_back(enInfo);
                }
                else
                {
                    float distance = app::Vector3_Distance(bonesMap[app::HumanBodyBones__Enum::Head].position, G::localPlayer->fields.m_goodPosition, NULL);
                    if (!isValidDistance(visible, distance))
                        continue;
                    auto enInfo = std::shared_ptr<EnemyInfo>(new EnemyInfo(visible, bonesMap, damageableBonesVec, enemyAgent, enemyName, distance));
                    enemiesTemp.push_back(enInfo);
                }
            }
        }

        G::enemyVecMtx.lock();
        enemies.clear();
        enemies = enemiesTemp;
        G::enemyVecMtx.unlock();

        G::enemyAimMtx.lock();
        enemiesAimbot.clear();
        enemiesAimbot = enemiesTemp;
        G::enemyAimMtx.unlock();
    }

    void _SpawnEnemy(int id, app::AgentMode__Enum agentMode)
    {
        app::EnemyAllocator* enemyAllocator = (*app::EnemyAllocator__TypeInfo)->static_fields->Current;
        app::PlayerAgent* localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        app::Agent* localPlayerAgent = reinterpret_cast<app::Agent*>(localPlayer);
        app::Quaternion playerRotation = app::Agent_get_Rotation(localPlayerAgent, NULL);
        app::AIG_CourseNode* courseNode = localPlayer->fields.m_courseNode;

        app::Vector3 screenCenter;
        screenCenter.x = G::screenWidth / 2.0f;
        screenCenter.y = G::screenHeight / 2.0f;
        screenCenter.z = 0;
        app::Ray screenCenterRay = app::Camera_ScreenPointToRay_2(G::mainCamera, screenCenter, NULL);
        app::RaycastHit raycastHit;
        if (app::Physics_Raycast_14(screenCenterRay, &raycastHit, 200, NULL))
        {
            app::EnemyAllocator_ResetAllowedToSpawn(NULL);
            app::EnemyAllocator_SpawnEnemy(enemyAllocator, id, courseNode, agentMode, raycastHit.m_Point, playerRotation, NULL, NULL, NULL);
        }
    }

    void RefreshEnemyAgents()
    {
        G::callbacks.push([] { _RefreshEnemyAgents(); });
    }

    void SpawnEnemy(int id, app::AgentMode__Enum agentMode = app::AgentMode__Enum::Off)
    {
        G::callbacks.push([id, agentMode] { _SpawnEnemy(id, agentMode); });
    }
}

