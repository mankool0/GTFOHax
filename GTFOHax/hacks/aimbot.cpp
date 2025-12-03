#include "aimbot.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "enemy.h"
#include "utils/math.h"

namespace Aimbot
{
    Settings settings;
    app::Vector3 silentAimBone;
    bool isSilentAiming = false;
    app::EnemyAgent* targetEnemy = nullptr;
    app::Dam_EnemyDamageLimb* targetLimb = nullptr;
    
    // Hit ghost storage
    std::vector<HitGhost> hitGhosts;
    std::mutex hitGhostMtx;
    
    // Bullet ray storage
    std::vector<BulletRay> bulletRays;
    std::mutex bulletRayMtx;
    
    void AddHitGhost(const std::map<app::HumanBodyBones__Enum, Enemy::Bone>& bones)
    {
        if (!settings.hitGhostEnabled || bones.empty())
            return;
            
        std::lock_guard<std::mutex> lock(hitGhostMtx);
        hitGhosts.emplace_back(bones, settings.hitGhostDuration);
        
        // Limit max number of ghosts to prevent memory issues
        if (hitGhosts.size() > 50)
        {
            hitGhosts.erase(hitGhosts.begin());
        }
    }
    
    void CleanupHitGhosts()
    {
        std::lock_guard<std::mutex> lock(hitGhostMtx);
        hitGhosts.erase(
            std::remove_if(hitGhosts.begin(), hitGhosts.end(), 
                [](const HitGhost& ghost) { return ghost.IsExpired(); }),
            hitGhosts.end()
        );
    }
    
    void RenderHitGhosts()
    {
        if (!settings.hitGhostEnabled || !settings.magicBullet)
            return;
            
        CleanupHitGhosts();
        
        std::lock_guard<std::mutex> lock(hitGhostMtx);
        
        for (const HitGhost& ghost : hitGhosts)
        {
            float alpha = ghost.GetAlpha();
            if (alpha <= 0.0f)
                continue;
                
            // Create color with fading alpha
            ImVec4 color = settings.hitGhostColor;
            color.w *= alpha;
            ImU32 colorU32 = ImGui::GetColorU32(color);
            
            // Draw skeleton bones for each group
            for (const auto& boneGroup : Enemy::skeletonBones)
            {
                app::Vector3 prevPos = {0, 0, 0};
                bool hasPrev = false;
                
                for (const auto& boneType : boneGroup)
                {
                    auto it = ghost.skeletonPositions.find(boneType);
                    if (it == ghost.skeletonPositions.end())
                        continue;
                        
                    app::Vector3 curPos = it->second;
                    
                    if (hasPrev && !(prevPos.x == 0.0f && prevPos.y == 0.0f && prevPos.z == 0.0f))
                    {
                        // Draw bone line
                        ImVec2 screenStart, screenEnd;
                        if (Math::WorldToScreen(prevPos, screenStart) && 
                            Math::WorldToScreen(curPos, screenEnd))
                        {
                            ImGui::GetBackgroundDrawList()->AddLine(
                                screenStart, screenEnd, colorU32, settings.hitGhostThickness);
                        }
                    }
                    
                    prevPos = curPos;
                    hasPrev = true;
                }
            }
        }
    }
    
    void AddBulletRay(app::Vector3 startPos, app::Vector3 endPos)
    {
        if (!settings.bulletRayEnabled)
            return;
            
        std::lock_guard<std::mutex> lock(bulletRayMtx);
        bulletRays.emplace_back(startPos, endPos, settings.bulletRayDuration);
        
        // Limit max number of rays to prevent memory issues
        if (bulletRays.size() > 50)
        {
            bulletRays.erase(bulletRays.begin());
        }
    }
    
    void CleanupBulletRays()
    {
        std::lock_guard<std::mutex> lock(bulletRayMtx);
        bulletRays.erase(
            std::remove_if(bulletRays.begin(), bulletRays.end(),
                [](const BulletRay& ray) { return ray.IsExpired(); }),
            bulletRays.end()
        );
    }
    
    void RenderBulletRays()
    {
        if (!settings.bulletRayEnabled || !settings.magicBullet)
            return;
            
        CleanupBulletRays();
        
        std::lock_guard<std::mutex> lock(bulletRayMtx);
        
        for (const BulletRay& ray : bulletRays)
        {
            float alpha = ray.GetAlpha();
            if (alpha <= 0.0f)
                continue;
                
            // Create color with fading alpha
            ImVec4 color = settings.bulletRayColor;
            color.w *= alpha;
            ImU32 colorU32 = ImGui::GetColorU32(color);
            
            // Convert world positions to screen positions and draw line
            // Need to copy to non-const variables for WorldToScreen
            app::Vector3 startPos = ray.startPos;
            app::Vector3 endPos = ray.endPos;
            ImVec2 screenStart, screenEnd;
            if (Math::WorldToScreen(startPos, screenStart) && 
                Math::WorldToScreen(endPos, screenEnd))
            {
                ImGui::GetBackgroundDrawList()->AddLine(
                    screenStart, screenEnd, colorU32, settings.bulletRayThickness);
            }
        }
    }

    // Get the real-time position of the target bone for magic bullet
    // This is called at the moment of firing to get the current position
    // Thread-safe: all pointer access happens while holding the lock
    app::Vector3 GetCurrentTargetPosition()
    {
        // Take a snapshot of the target pointers under lock protection
        app::EnemyAgent* localTargetEnemy = nullptr;
        app::Dam_EnemyDamageLimb* localTargetLimb = nullptr;
        app::Vector3 cachedPos = silentAimBone;
        
        {
            std::lock_guard<std::mutex> lock(G::enemyAimMtx);
            
            if (targetEnemy == nullptr)
                return silentAimBone;
            
            // Validate that targetEnemy still exists in the enemy list
            bool enemyStillValid = false;
            for (const auto& enemyInfo : Enemy::enemiesAimbot)
            {
                if (enemyInfo->enemyAgent == targetEnemy)
                {
                    enemyStillValid = true;
                    break;
                }
            }
            
            if (!enemyStillValid)
            {
                // Enemy no longer exists, clear the pointer and use cached position
                targetEnemy = nullptr;
                targetLimb = nullptr;
                return silentAimBone;
            }
            
            // Copy pointers while still holding lock - the enemy list guarantees
            // these pointers are valid as long as we hold the lock
            localTargetEnemy = targetEnemy;
            localTargetLimb = targetLimb;
            cachedPos = silentAimBone;
            
            // Perform all game API calls while holding the lock to prevent
            // the enemy from being removed mid-access
            auto enemyDamage = reinterpret_cast<app::Dam_SyncedDamageBase*>(localTargetEnemy->fields.Damage);
            if (enemyDamage == nullptr || enemyDamage->fields._Health_k__BackingField <= 0.0f)
            {
                // Enemy is dead or invalid, use cached position
                return cachedPos;
            }
            
            if (localTargetLimb != nullptr)
            {
                // Get the current position of the damage limb
                return app::Dam_EnemyDamageLimb_get_DamageTargetPos(localTargetLimb, NULL);
            }
            else
            {
                // Fallback to enemy position if no specific limb
                return app::EnemyAgent_get_Position(localTargetEnemy, NULL);
            }
        }
    }

    void RunAimbot()
    {
        if (!settings.toggleKey.isToggled())
        {
            isSilentAiming = false;
            targetEnemy = nullptr;
            targetLimb = nullptr;
            return;
        }

        if (settings.holdOnly && !settings.holdKey.isDown())
        {
            isSilentAiming = false;
            targetEnemy = nullptr;
            targetLimb = nullptr;
            return;
        }

        app::Vector3 localEyePos = G::localPlayer->fields.m_eyePosition;
        app::LocalPlayerAgent* localPlayerAgent = reinterpret_cast<app::LocalPlayerAgent*>(G::localPlayer);
        app::Vector3 playerForwardVec = app::FPSCamera_get_Forward(localPlayerAgent->fields.m_FPSCamera, NULL);

        G::enemyAimMtx.lock();
        std::vector<std::shared_ptr<Enemy::EnemyInfo>> enemies = Enemy::enemiesAimbot;
        G::enemyAimMtx.unlock();
        
        // TODO: Time this? Is it worth to sort and then loop again for convenience
        // or should it all be done in a single loop?
        switch (settings.priority)
        {
            case Health:
            {
                std::sort(enemies.begin(), enemies.end(), [](const std::shared_ptr<Enemy::EnemyInfo> lhs, const std::shared_ptr<Enemy::EnemyInfo> rhs) {
                    auto enemyDamageLhs = reinterpret_cast<app::Dam_SyncedDamageBase*>(lhs->enemyAgent->fields.Damage);
                    auto enemyDamageRhs = reinterpret_cast<app::Dam_SyncedDamageBase*>(rhs->enemyAgent->fields.Damage);
                    return enemyDamageLhs->fields._Health_k__BackingField < enemyDamageRhs->fields._Health_k__BackingField;
                    });
                break;
            }
            case Distance:
            {
                std::sort(enemies.begin(), enemies.end(), [](const std::shared_ptr<Enemy::EnemyInfo> lhs, const std::shared_ptr<Enemy::EnemyInfo> rhs) {
                    return lhs->distance < rhs->distance;
                    });
                break;
            }
            case FOV:
            {
                std::sort(enemies.begin(), enemies.end(), [localEyePos, playerForwardVec](const std::shared_ptr<Enemy::EnemyInfo> lhs, const std::shared_ptr<Enemy::EnemyInfo> rhs) {
                    app::Vector3 lhsPos = lhs->useFallback ? lhs->fallbackBone.position : lhs->skeletonBones.at(app::HumanBodyBones__Enum::Head).position;
                    app::Vector3 lhsVec = Math::Vector3Sub(lhsPos, localEyePos);

                    app::Vector3 rhsPos = rhs->useFallback ? rhs->fallbackBone.position : rhs->skeletonBones.at(app::HumanBodyBones__Enum::Head).position;
                    app::Vector3 rhsVec = Math::Vector3Sub(rhsPos, localEyePos);

                    return app::Vector3_Angle(playerForwardVec, lhsVec, NULL) < app::Vector3_Angle(playerForwardVec, rhsVec, NULL);
                    });
                break;
            }
        }

        bool foundEnemy = false;
        for (auto it = enemies.begin(); it != enemies.end(); ++it)
        {
            Enemy::EnemyInfo* enemyInfo = (*it).get();
            if ((settings.visibleOnly && !enemyInfo->visible)
                || (settings.maxDistance < enemyInfo->distance))
                continue;
            
            auto enemyDamage = reinterpret_cast<app::Dam_SyncedDamageBase*>(enemyInfo->enemyAgent->fields.Damage);
            if (enemyDamage->fields._Health_k__BackingField <= 0.0f)
                continue;

            Enemy::Bone bestBone;
            bool foundBone = false;
            if (!enemyInfo->damageableBones.empty())
            {
                for (Enemy::Bone& bone : enemyInfo->damageableBones)
                {
                    if (settings.visibleOnly && !bone.visible)
                        continue;
                    if (!settings.aimAtArmor && bone.limbType == app::eLimbDamageType__Enum::Armor)
                        continue;

                    app::Vector3 boneDelta = Math::Vector3Sub(bone.position, localEyePos);
                    float angle = app::Vector3_Angle(playerForwardVec, boneDelta, NULL);
                    if (angle > (settings.aimFov / 2))
                        continue;

                    if (enemyInfo->skeletonBones.contains(app::HumanBodyBones__Enum::Head))
                    {
                        // Destroyed head bone can't be shot at. Make sure this isn't that
                        Enemy::Bone headBone = enemyInfo->skeletonBones[app::HumanBodyBones__Enum::Head]; // TODO: Fix map issue
                        if (headBone.destroyed && Math::Vector3Eq(headBone.position, bone.position))
                            continue;
                    }


                    if (!foundBone)
                    {
                        bestBone = bone;
                        foundBone = true;
                        continue;
                    }
                    
                    if ((bestBone.limbType == app::eLimbDamageType__Enum::Armor && (bone.limbType == app::eLimbDamageType__Enum::Normal || bone.limbType == app::eLimbDamageType__Enum::Weakspot))
                        || (bestBone.limbType == app::eLimbDamageType__Enum::Normal && bone.limbType == app::eLimbDamageType__Enum::Weakspot))
                    {
                        // Current bone limbtype is better
                        bestBone = bone;
                        continue;
                    }
                    
                    if (bestBone.limbType == bone.limbType &&
                        (bone.health > bestBone.health))
                    {
                        bestBone = bone;
                        continue;
                    }

                }
            }
            else if (enemyInfo->useFallback && !(settings.visibleOnly && !enemyInfo->fallbackBone.visible))
            {
                bestBone = enemyInfo->fallbackBone;
                foundBone = true;
            }

            if (!foundBone)
                continue;

            if (settings.silentAim)
            {
                silentAimBone = bestBone.position;
                // Store target info for real-time position update (magic bullet fix)
                targetEnemy = enemyInfo->enemyAgent;
                targetLimb = bestBone.limbPtr;  // Will be nullptr if not a damage limb
            }
            else
            {
                app::Vector3 boneVec = Math::Vector3Sub(bestBone.position, localEyePos);
                app::Vector3 aimVec = app::Vector3_Lerp(boneVec, playerForwardVec, settings.smoothing, NULL);
                app::FPSCamera_SetLookDirection(localPlayerAgent->fields.m_FPSCamera, aimVec, NULL);
            }
            foundEnemy = true;
            break; // Found what to aim at, stop looping through enemies
        }

        if (foundEnemy && settings.silentAim)
        {
            isSilentAiming = true;
        }
        else
        {
            isSilentAiming = false;
            targetEnemy = nullptr;
            targetLimb = nullptr;
        }
    }
    
    bool IsLockedTarget(app::EnemyAgent* enemy)
    {
        if (enemy == nullptr)
            return false;
        return isSilentAiming && targetEnemy == enemy;
    }
}