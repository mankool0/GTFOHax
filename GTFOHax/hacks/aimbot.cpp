#include "aimbot.h"
#include <iostream>
#include <string>
#include "enemy.h"
#include "..\math.h"

namespace Aimbot
{
    Settings settings;
    app::Vector3 silentAimBone;
    bool isSilentAiming = false;

    void RunAimbot()
    {
        if (!settings.toggleKey.isToggled())
        {
            isSilentAiming = false;
            return;
        }

        if (settings.holdOnly && !settings.holdKey.isDown())
        {
            isSilentAiming = false;
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
                        Enemy::Bone headBone = enemyInfo->skeletonBones[app::HumanBodyBones__Enum::Head];
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
            isSilentAiming = true;
        else
            isSilentAiming = false;
    }
}