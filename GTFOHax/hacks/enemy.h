#pragma once
#include "..\globals.h"
#include <vector>
#include <map>

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
        std::map<app::HumanBodyBones__Enum, Bone> skeletonBones;
        std::vector<Bone> damageableBones;
        Bone fallbackBone;
        std::string enemyObjectName;
        float distance;
        bool useFallback;

        EnemyInfo(bool visible, std::map<app::HumanBodyBones__Enum, Bone> skeletonBones, std::vector<Bone> damageableBones, app::EnemyAgent* enemyAgent, std::string enemyObjectName, float distance)
        {
            this->visible = visible;
            this->skeletonBones = skeletonBones;
            this->damageableBones = damageableBones;
            this->enemyAgent = enemyAgent;
            this->enemyObjectName = enemyObjectName;
            this->distance = distance;
            this->useFallback = false;
        }
        
        EnemyInfo(bool visible, std::map<app::HumanBodyBones__Enum, Bone> skeletonBones, std::vector<Bone> damageableBones, app::EnemyAgent* enemyAgent, std::string enemyObjectName, float distance, Bone fallbackBone)
        {
            this->visible = visible;
            this->skeletonBones = skeletonBones;
            this->damageableBones = damageableBones;
            this->enemyAgent = enemyAgent;
            this->enemyObjectName = enemyObjectName;
            this->distance = distance;
            this->fallbackBone = fallbackBone;
            this->useFallback = true;
        }
    };

    extern std::vector<std::shared_ptr<EnemyInfo>> enemies;
    extern std::vector<std::shared_ptr<EnemyInfo>> enemiesAimbot;
    extern std::map<std::string, int> enemyIDs;
    extern std::vector<std::string> enemyNames;

    void _RefreshEnemyAgents();
    void RefreshEnemyAgents();
    void SpawnEnemy(int id, app::AgentMode__Enum agentMode);
}