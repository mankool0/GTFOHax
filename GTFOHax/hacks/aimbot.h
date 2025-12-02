#pragma once
#include "globals.h"
#include "InputUtil.h"
#include <vector>
#include <chrono>
#include <map>
#include "enemy.h"

namespace Aimbot
{
    // Hit ghost effect - shows a fading skeleton at hit location
    struct HitGhost
    {
        std::map<app::HumanBodyBones__Enum, app::Vector3> skeletonPositions;  // Snapshot of skeleton at hit time
        std::chrono::steady_clock::time_point hitTime;  // When the hit occurred
        float duration;  // How long to display (seconds)
        
        HitGhost(const std::map<app::HumanBodyBones__Enum, Enemy::Bone>& bones, float duration = 0.3f)
            : hitTime(std::chrono::steady_clock::now()), duration(duration)
        {
            for (const auto& [boneType, bone] : bones)
            {
                skeletonPositions[boneType] = bone.position;
            }
        }
        
        // Get alpha (1.0 = fully visible, 0.0 = invisible)
        float GetAlpha() const
        {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - hitTime).count();
            if (elapsed >= duration) return 0.0f;
            return 1.0f - (elapsed / duration);
        }
        
        bool IsExpired() const
        {
            return GetAlpha() <= 0.0f;
        }
    };
    
    extern std::vector<HitGhost> hitGhosts;
    extern std::mutex hitGhostMtx;
    
    // Add a new hit ghost
    void AddHitGhost(const std::map<app::HumanBodyBones__Enum, Enemy::Bone>& bones);
    // Clean up expired ghosts
    void CleanupHitGhosts();
    // Render all hit ghosts
    void RenderHitGhosts();
    // Check if an enemy is the current aimbot target
    bool IsLockedTarget(app::EnemyAgent* enemy);
    struct Angle
    {
        float yaw, yitch = 0.0f;
    };

    //static const char* AimTypeItems[] = { "Best", "Bone" };
    //enum AimType
    //{
    //    Best = 0,
    //    Bone,
    //};

    static const char* EnemyPriorityItems[] = { "Health", "Distance", "FOV" };
    static const char* SelectedEnemyPriorityItem = NULL;
    enum EnemyPriority
    {
        Health = 0,
        Distance,
        FOV,
    };

    struct Settings
    {
        KeyBindToggle toggleKey;
        bool holdOnly = false;
        KeyBind holdKey;
        bool silentAim = true;
        bool magicBullet = false;
        bool hitGhostEnabled = true;      // Show hit ghost effect
        float hitGhostDuration = 0.6f;    // Duration in seconds
        ImVec4 hitGhostColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // Red color for hit ghost
        float hitGhostThickness = 2.0f;   // Line thickness
        bool targetHighlight = true;       // Highlight locked target in ESP
        ImVec4 targetHighlightColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);  // Orange color for locked target
        bool targetHighlightMarker = true; // Show [LOCKED] marker on target
        bool visibleOnly = true;
        bool aimAtArmor = false;
        float maxDistance = 250.0f;
        float smoothing = 0.0f;
        float aimFov = 15.0f;
        ImVec4 aimFovColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        bool renderFOV = true;
        //AimType aimType = AimType::Best;
        EnemyPriority priority = EnemyPriority::FOV;
    };

    extern app::Vector3 silentAimBone;
    extern bool isSilentAiming;
    extern Settings settings;
    
    // Store the target enemy info for real-time position update during magic bullet
    extern app::EnemyAgent* targetEnemy;
    extern app::Dam_EnemyDamageLimb* targetLimb;  // The specific damage limb being aimed at
    
    // Get the current position of the target bone in real-time
    app::Vector3 GetCurrentTargetPosition();

    void RunAimbot();
}
