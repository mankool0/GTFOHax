#pragma once
#include "globals.h"
#include <vector>
#include <map>
#include <atomic>

namespace Enemy
{
    struct Bone
    {
        app::Vector3 position = app::Vector3(0.0f, 0.0f, 0.0f);
        bool visible = false;
        bool damageable = false;
        bool destroyed = false;
        app::eLimbDamageType__Enum limbType;
        float health;
        app::Dam_EnemyDamageLimb* limbPtr = nullptr;  // Pointer to the original damage limb for real-time position
    };

    static const std::vector<app::HumanBodyBones__Enum> WantedBones =
    {
        app::HumanBodyBones__Enum::Head, app::HumanBodyBones__Enum::Neck,
        app::HumanBodyBones__Enum::RightUpperArm, app::HumanBodyBones__Enum::RightLowerArm, app::HumanBodyBones__Enum::RightHand,
        app::HumanBodyBones__Enum::LeftUpperArm, app::HumanBodyBones__Enum::LeftLowerArm, app::HumanBodyBones__Enum::LeftHand,
        app::HumanBodyBones__Enum::Chest, app::HumanBodyBones__Enum::Spine, app::HumanBodyBones__Enum::Hips,
        app::HumanBodyBones__Enum::RightUpperLeg, app::HumanBodyBones__Enum::RightLowerLeg, app::HumanBodyBones__Enum::RightFoot,
        app::HumanBodyBones__Enum::LeftUpperLeg, app::HumanBodyBones__Enum::LeftLowerLeg, app::HumanBodyBones__Enum::LeftFoot
    };

    static const std::vector<app::HumanBodyBones__Enum> AllBones =
    {
        app::HumanBodyBones__Enum::Hips, app::HumanBodyBones__Enum::LeftUpperLeg, app::HumanBodyBones__Enum::RightUpperLeg,
        app::HumanBodyBones__Enum::LeftLowerLeg, app::HumanBodyBones__Enum::RightLowerLeg, app::HumanBodyBones__Enum::LeftFoot,
        app::HumanBodyBones__Enum::RightFoot, app::HumanBodyBones__Enum::Spine, app::HumanBodyBones__Enum::Chest,
        app::HumanBodyBones__Enum::UpperChest, app::HumanBodyBones__Enum::Neck, app::HumanBodyBones__Enum::Head,
        app::HumanBodyBones__Enum::LeftShoulder, app::HumanBodyBones__Enum::RightShoulder, app::HumanBodyBones__Enum::LeftUpperArm,
        app::HumanBodyBones__Enum::RightUpperArm, app::HumanBodyBones__Enum::LeftLowerArm, app::HumanBodyBones__Enum::RightLowerArm,
        app::HumanBodyBones__Enum::LeftHand, app::HumanBodyBones__Enum::RightHand, app::HumanBodyBones__Enum::LeftToes,
        app::HumanBodyBones__Enum::RightToes, app::HumanBodyBones__Enum::LeftEye, app::HumanBodyBones__Enum::RightEye,
        app::HumanBodyBones__Enum::Jaw, app::HumanBodyBones__Enum::LeftThumbProximal, app::HumanBodyBones__Enum::LeftThumbIntermediate,
        app::HumanBodyBones__Enum::LeftThumbDistal, app::HumanBodyBones__Enum::LeftIndexProximal, app::HumanBodyBones__Enum::LeftIndexIntermediate,
        app::HumanBodyBones__Enum::LeftIndexDistal, app::HumanBodyBones__Enum::LeftMiddleProximal, app::HumanBodyBones__Enum::LeftMiddleIntermediate,
        app::HumanBodyBones__Enum::LeftMiddleDistal, app::HumanBodyBones__Enum::LeftRingProximal, app::HumanBodyBones__Enum::LeftRingIntermediate,
        app::HumanBodyBones__Enum::LeftRingDistal, app::HumanBodyBones__Enum::LeftLittleProximal, app::HumanBodyBones__Enum::LeftLittleIntermediate,
        app::HumanBodyBones__Enum::LeftLittleDistal, app::HumanBodyBones__Enum::RightThumbProximal, app::HumanBodyBones__Enum::RightThumbIntermediate,
        app::HumanBodyBones__Enum::RightThumbDistal, app::HumanBodyBones__Enum::RightIndexProximal, app::HumanBodyBones__Enum::RightIndexIntermediate,
        app::HumanBodyBones__Enum::RightIndexDistal, app::HumanBodyBones__Enum::RightMiddleProximal, app::HumanBodyBones__Enum::RightMiddleIntermediate,
        app::HumanBodyBones__Enum::RightMiddleDistal, app::HumanBodyBones__Enum::RightRingProximal, app::HumanBodyBones__Enum::RightRingIntermediate,
        app::HumanBodyBones__Enum::RightRingDistal, app::HumanBodyBones__Enum::RightLittleProximal, app::HumanBodyBones__Enum::RightLittleIntermediate,
        app::HumanBodyBones__Enum::RightLittleDistal
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonHead =
    {
        app::HumanBodyBones__Enum::Head, app::HumanBodyBones__Enum::Neck
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonRightArm = 
    {
        app::HumanBodyBones__Enum::Neck, app::HumanBodyBones__Enum::RightUpperArm, app::HumanBodyBones__Enum::RightLowerArm, app::HumanBodyBones__Enum::RightHand
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonLeftArm =
    {
        app::HumanBodyBones__Enum::Neck, app::HumanBodyBones__Enum::LeftUpperArm, app::HumanBodyBones__Enum::LeftLowerArm, app::HumanBodyBones__Enum::LeftHand
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonSpine = 
    {
        app::HumanBodyBones__Enum::Neck, app::HumanBodyBones__Enum::Chest, app::HumanBodyBones__Enum::Spine, app::HumanBodyBones__Enum::Hips 
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonRightLeg = 
    {
        app::HumanBodyBones__Enum::Hips, app::HumanBodyBones__Enum::RightUpperLeg, app::HumanBodyBones__Enum::RightLowerLeg, app::HumanBodyBones__Enum::RightFoot
    };

    static std::vector<app::HumanBodyBones__Enum> const skeletonLeftLeg =
    {
        app::HumanBodyBones__Enum::Hips, app::HumanBodyBones__Enum::LeftUpperLeg, app::HumanBodyBones__Enum::LeftLowerLeg, app::HumanBodyBones__Enum::LeftFoot
    };

    static std::vector<std::vector<app::HumanBodyBones__Enum>> skeletonBones =
    {
        skeletonHead, skeletonRightArm, skeletonLeftArm, skeletonSpine, skeletonRightLeg, skeletonLeftLeg
    };

    struct EnemyInfo
    {
        bool visible;
        app::EnemyAgent* enemyAgent;
        
        Bone bones[64];
        bool hasBone[64];
        
        ImVec2 screenPositions[64];
        bool screenPositionsValid[64];

        Bone damageableBones[32];
        int  damageableBoneCount;
        Bone fallbackBone;
        std::string enemyObjectName;
        float distance;
        bool useFallback;

        // Efficient constructor
        EnemyInfo() : visible(false), enemyAgent(nullptr), distance(0), useFallback(false), damageableBoneCount(0)
        {
            memset(hasBone, 0, sizeof(hasBone));
            memset(screenPositionsValid, 0, sizeof(screenPositionsValid));
        }

        inline const Bone* getBone(app::HumanBodyBones__Enum boneType) const
        {
            int idx = static_cast<int>(boneType);
            if (idx >= 0 && idx < 64 && hasBone[idx])
                return &bones[idx];
            return nullptr;
        }
    };

    using EnemyVec = std::vector<std::shared_ptr<EnemyInfo>>;
    extern std::atomic<std::shared_ptr<EnemyVec>> enemies;
    extern std::atomic<std::shared_ptr<EnemyVec>> enemiesAimbot;
    extern std::map<std::string, int> enemyIDs;
    extern std::vector<std::string> enemyNames;
    
    // Position tracking for calculating movement direction
    struct EnemyPositionHistory
    {
        app::Vector3 previousPosition;
        app::Vector3 currentPosition;
        app::Vector3 movementDirection;  // Normalized direction
        bool hasValidDirection;
    };
    extern std::map<app::EnemyAgent*, EnemyPositionHistory> enemyPositionHistory;
    extern std::mutex enemyPositionHistoryMtx;
    
    // Get the movement direction of an enemy (returns normalized vector, or zero vector if not moving)
    app::Vector3 GetEnemyMovementDirection(app::EnemyAgent* enemy);

    void _RefreshEnemyAgents();
    void RefreshEnemyAgents();
    void SpawnEnemy(int id, app::AgentMode__Enum agentMode);
}