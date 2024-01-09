#pragma once
#include "..\globals.h"
#include "..\InputUtil.h"

namespace Aimbot
{
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

    void RunAimbot();
}
