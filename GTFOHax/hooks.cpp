#define USE_DETOURS

#include "hooks.h"
#include "menu.h"
#include "hacks\player.h"
#include "framework\helpers.h"
#include "hacks\esp.h"
#include "fonts\fonts.h"

#include <iostream>
#include <string>
#include <misc/freetype/imgui_freetype.h>
#include "hacks/enemy.h"
#include "math.h"
#include "hacks/aimbot.h"

#ifdef USE_DETOURS
#include <detours/detours.h>
#else
#include "../kiero/minhook/include/MinHook.h"
#endif // USE_DETOURS

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool hookFailed = false;

std::string HookErrorToString(LONG code)
{
    switch (code)
    {
#ifdef USE_DETOURS
    case ERROR_INVALID_BLOCK:
        return "ERROR_INVALID_BLOCK";
    case ERROR_INVALID_HANDLE:
        return "ERROR_INVALID_HANDLE";
    case ERROR_INVALID_OPERATION:
        return "ERROR_INVALID_OPERATION";
    case ERROR_NOT_ENOUGH_MEMORY:
        return "ERROR_NOT_ENOUGH_MEMORY";
    case NO_ERROR:
        return "NO_ERROR";
#else
    case MH_UNKNOWN:
        return "MH_UNKNOWN";
    case MH_OK:
        return "MH_OK";
    case MH_ERROR_ALREADY_INITIALIZED:
        return "MH_ERROR_ALREADY_INITIALIZED";
    case MH_ERROR_NOT_INITIALIZED:
        return "MH_ERROR_NOT_INITIALIZED";
    case MH_ERROR_ALREADY_CREATED:
        return "MH_ERROR_ALREADY_CREATED";
    case MH_ERROR_NOT_CREATED:
        return "MH_ERROR_NOT_CREATED";
    case MH_ERROR_ENABLED:
        return "MH_ERROR_ENABLED";
    case MH_ERROR_DISABLED:
        return "MH_ERROR_DISABLED";
    case MH_ERROR_NOT_EXECUTABLE:
        return "MH_ERROR_NOT_EXECUTABLE";
    case MH_ERROR_UNSUPPORTED_FUNCTION:
        return "MH_ERROR_UNSUPPORTED_FUNCTION";
    case MH_ERROR_MEMORY_ALLOC:
        return "MH_ERROR_MEMORY_ALLOC";
    case MH_ERROR_MEMORY_PROTECT:
        return "MH_ERROR_MEMORY_PROTECT";
    case MH_ERROR_MODULE_NOT_FOUND:
        return "MH_ERROR_MODULE_NOT_FOUND";
    case MH_ERROR_FUNCTION_NOT_FOUND:
        return "MH_ERROR_FUNCTION_NOT_FOUND";
#endif // USE_DETOURS
    default:
        return std::to_string(code);
    }
}

#ifdef USE_DETOURS
std::vector<std::tuple<std::string, PVOID*, PVOID>> hooks;

void HookAttach(PVOID* ppPointer, PVOID pDetour, std::string functionName)
{
    LONG code = DetourAttach(ppPointer, pDetour);
    if (code != NO_ERROR)
    {
        std::string error = "DetourAttach Failed On: " + functionName + " With " + HookErrorToString(code);
        il2cppi_log_write(error);
        hookFailed = true;
    }
}

void HookDetach(PVOID* ppPointer, PVOID pDetour, std::string functionName)
{
    LONG code = DetourDetach(ppPointer, pDetour);
    if (code != NO_ERROR)
    {
        std::string error = "DetourDetach Failed On: " + functionName + "With " + HookErrorToString(code);
        il2cppi_log_write(error);
    }
}

#define HOOKATTACH(fun) (HookAttach(&(PVOID&)app::fun, Hooks:: ## hk ## fun, #fun))
#define HOOKDETACH(fun) (HookDetach(&(PVOID&)app::fun, Hooks:: ## hk ## fun, #fun))

#else
typedef void (*hookFunc)(void);
std::map<std::string, hookFunc> fpMap;
void HookAttach(PVOID ppPointer, PVOID pDetour, PVOID* fpOrig, std::string functionName)
{
    MH_STATUS codeCreate, codeEnable = MH_UNKNOWN;
    codeCreate = MH_CreateHook(ppPointer, pDetour, fpOrig);
    if (codeCreate == MH_OK)
    {    
        codeEnable = MH_EnableHook(ppPointer);
    }

    if (codeCreate != MH_OK || codeEnable != MH_OK)
    {    
        std::string error = "HookAttach Failed On: " + functionName + " With Create: " + HookErrorToString(codeCreate) + " Enable: " + HookErrorToString(codeEnable);
        il2cppi_log_write(error);
        hookFailed = true;
    }
}

void HookDetach(PVOID ppPointer, std::string functionName)
{
    MH_STATUS code = MH_RemoveHook(ppPointer);
    if (code != MH_OK)
    {
        std::string error = "HookDetach Failed On: " + functionName + "With " + HookErrorToString(code);
        il2cppi_log_write(error);
    }
}

#define HOOKATTACH(fun) void (*fp ## fun)(void); \
                        HookAttach(app::fun, &Hooks:: ## hk ## fun, reinterpret_cast<PVOID*>(&fp ## fun), #fun); \
                        fpMap[#fun] = (fp ## fun)
#define HOOKDETACH(fun) (HookDetach(app::fun, #fun))

#endif // USE_DETOURS

void Hooks::InitHooks()
{
    MethodInfo* methodInfo = NULL;
    while (methodInfo == NULL)
    {
        methodInfo = *app::GameDataBlockBase_1_EnemyDataBlock__GetAllBlocks__MethodInfo;
        Sleep(100);
    }
    auto allBlocks = app::GameDataBlockBase_1_EnemyDataBlock__GetAllBlocks(methodInfo);
    for (int i = 0; i < allBlocks->max_length; i++)
    {
        auto block = allBlocks->vector[i];
        auto blockName = il2cppi_to_string(block->fields._._name_k__BackingField);
        auto blockPersID = block->fields._._persistentID_k__BackingField;
        if (blockName == "Striker_Boss" || blockName == "Cocoon" || blockName == "Squidward" || blockName == "SquidBoss_Big")
            continue; //Ignore enemies that can't be spawned
        std::replace(blockName.begin(), blockName.end(), '_', ' ');
        Enemy::enemyNames.push_back(blockName);
        Enemy::enemyIDs.insert(std::pair<std::string, int>(blockName, blockPersID));       
    }
    std::sort(Enemy::enemyNames.begin(), Enemy::enemyNames.end());

    bool hooked = false;
    while (!hooked)
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&G::oPresent, (void*)hkPresent);
            hooked = true;
        }
    }

#ifdef USE_DETOURS
    il2cppi_log_write("Using Detours for hooking");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
#else
    il2cppi_log_write("Using MinHook for hooking");
#endif // USE_DETOURS

    HOOKATTACH(Dam_PlayerDamageBase_OnIncomingDamage);
    HOOKATTACH(Dam_PlayerDamageBase_ModifyInfection);
    HOOKATTACH(Dam_PlayerDamageLocal_Hitreact);
    HOOKATTACH(FPS_RecoilSystem_ApplyRecoil);
    HOOKATTACH(Weapon_CastWeaponRay);
    HOOKATTACH(Weapon_CastWeaponRay_1);
    HOOKATTACH(PlayerAmmoStorage_UpdateBulletsInPack);
    HOOKATTACH(BulletWeapon_Fire);
    HOOKATTACH(Shotgun_Fire);
    HOOKATTACH(GlueGun_Updatepressure);
    HOOKATTACH(GlueGun_UpdateRecharging);
    HOOKATTACH(HackingMinigame_TimingGrid_StartGame);
    HOOKATTACH(BulletWeaponArchetype_Update);
    HOOKATTACH(ArtifactPickup_Core_Setup);
    HOOKATTACH(CommodityPickup_Core_Setup);
    HOOKATTACH(ConsumablePickup_Core_Setup);
    HOOKATTACH(CarryItemPickup_Core_Setup);
    HOOKATTACH(KeyItemPickup_Core_Setup);
    HOOKATTACH(GenericSmallPickupItem_Core_Setup);
    HOOKATTACH(ResourcePackPickup_Setup);
    HOOKATTACH(LG_HSU_Setup);
    HOOKATTACH(LG_FunctionMarkerBuilder_SetupFunctionGO);

    HOOKATTACH(GameStateManager_ChangeState);

    HOOKATTACH(Cursor_set_lockState);
    HOOKATTACH(Cursor_set_visible);

    HOOKATTACH(LocalPlayerAgent_Update);

    HOOKATTACH(Dam_EnemyDamageBase_ProcessReceivedDamage);

#ifdef USE_DETOURS
    DetourTransactionCommit();
#endif // USE_DETOURS

    if (hookFailed)
        il2cppi_log_write("Failed Initializing Hooks");
    else
        il2cppi_log_write("Initialized Hooks");
}

void Hooks::RemoveHooks()
{
    G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (LONG_PTR)G::oWndProc);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

#ifdef USE_DETOURS
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
#else
    MH_DisableHook(MH_ALL_HOOKS);
#endif // USE_DETOURS

    HOOKDETACH(Dam_PlayerDamageBase_OnIncomingDamage);
    HOOKDETACH(Dam_PlayerDamageBase_ModifyInfection);
    HOOKDETACH(Dam_PlayerDamageLocal_Hitreact);
    HOOKDETACH(FPS_RecoilSystem_ApplyRecoil);
    HOOKDETACH(Weapon_CastWeaponRay);
    HOOKDETACH(Weapon_CastWeaponRay_1);
    HOOKDETACH(PlayerAmmoStorage_UpdateBulletsInPack);
    HOOKDETACH(BulletWeapon_Fire);
    HOOKDETACH(Shotgun_Fire);
    HOOKDETACH(GlueGun_Updatepressure);
    HOOKDETACH(GlueGun_UpdateRecharging);
    HOOKDETACH(HackingMinigame_TimingGrid_StartGame);
    HOOKDETACH(BulletWeaponArchetype_Update);
    HOOKDETACH(ArtifactPickup_Core_Setup);
    HOOKDETACH(CommodityPickup_Core_Setup);
    HOOKDETACH(ConsumablePickup_Core_Setup);
    HOOKDETACH(CarryItemPickup_Core_Setup);
    HOOKDETACH(KeyItemPickup_Core_Setup);
    HOOKDETACH(GenericSmallPickupItem_Core_Setup);
    HOOKDETACH(ResourcePackPickup_Setup);
    HOOKDETACH(LG_HSU_Setup);
    HOOKDETACH(LG_FunctionMarkerBuilder_SetupFunctionGO);

    HOOKDETACH(GameStateManager_ChangeState);

    HOOKDETACH(Cursor_set_lockState);
    HOOKDETACH(Cursor_set_visible);

    HOOKDETACH(LocalPlayerAgent_Update);

    HOOKDETACH(Dam_EnemyDamageBase_ProcessReceivedDamage);

#ifdef USE_DETOURS
    DetourTransactionCommit();
#endif // USE_DETOURS

    il2cppi_log_write("Detached Hooks");
}

bool Hooks::hkDam_PlayerDamageBase_OnIncomingDamage(app::Dam_PlayerDamageBase* __this, float damage, float originalDamage, app::Agent* source, MethodInfo* method)
{
    if (!Player::godmodeToggleKey.isToggled())
    {
#ifdef USE_DETOURS
        return app::Dam_PlayerDamageBase_OnIncomingDamage(__this, damage, originalDamage, source, method);
#else
        static auto fpOFunc = reinterpret_cast<bool(*)(app::Dam_PlayerDamageBase*, float, float, app::Agent*, MethodInfo*)>(fpMap["Dam_PlayerDamageBase_OnIncomingDamage"]);
        return fpOFunc(__this, damage, originalDamage, source, method);
#endif // USE_DETOURS
    }

    return false;
}

void Hooks::hkDam_PlayerDamageBase_ModifyInfection(app::Dam_PlayerDamageBase* __this, app::pInfection data, bool sync, bool updatePageMap, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::Dam_PlayerDamageBase_ModifyInfection(__this, data, sync, updatePageMap, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::Dam_PlayerDamageBase*, app::pInfection, bool, bool, MethodInfo*)>(fpMap["Dam_PlayerDamageBase_ModifyInfection"]);
    fpOFunc(__this, data, sync, updatePageMap, method);
#endif // USE_DETOURS
    if (Player::godmodeToggleKey.isToggled())
        __this->fields.Infection = 0.0f;
}

void Hooks::hkDam_PlayerDamageLocal_Hitreact(app::Dam_PlayerDamageLocal* __this, float damage, app::Vector3 direction, bool triggerCameraShake, bool triggerGenericDialog, bool pushPlayer, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::Dam_PlayerDamageLocal_Hitreact(__this, damage, direction, !Player::noShakeToggleKey.isToggled(), triggerGenericDialog, pushPlayer, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::Dam_PlayerDamageLocal*, float, app::Vector3, bool, bool, bool, MethodInfo*)>(fpMap["Dam_PlayerDamageLocal_Hitreact"]);
    fpOFunc(__this, damage, direction, !Player::noShakeToggleKey.isToggled(), triggerGenericDialog, pushPlayer, method);
#endif // USE_DETOURS
}

app::Vector2 Hooks::hkFPS_RecoilSystem_ApplyRecoil(app::FPS_RecoilSystem* __this, bool resetSimilarity, app::RecoilDataBlock* recoilData, MethodInfo* method)
{
    if (!Player::noRecoilToggleKey.isToggled())
    {
#ifdef USE_DETOURS
        return app::FPS_RecoilSystem_ApplyRecoil(__this, resetSimilarity, recoilData, method);
#else
        static auto fpOFunc = reinterpret_cast<app::Vector2(*)(app::FPS_RecoilSystem*, bool, app::RecoilDataBlock*, MethodInfo*)>(fpMap["FPS_RecoilSystem_ApplyRecoil"]);
        return fpOFunc(__this, resetSimilarity, recoilData, method);
#endif // USE_DETOURS
    }
    //std::cout << "ApplyRecoilHook" << std::endl;
    __this->fields.m_concussionShakeIntensity = 0.0f;
    __this->fields.m_concussionShakeFrequency = 0.0f;
    __this->fields.m_concussionShakeOffset = 0.0f;
    __this->fields.m_concussionShakeDuration = 0.0f;
    __this->fields.m_concussionShakeTimer = 0.0f;

    __this->fields.m_spring = 0.0f;
    __this->fields.m_damp = 0.0f;
    __this->fields.recoilDir.x = 0.0f;
    __this->fields.recoilDir.y = 0.0f;
    __this->fields.m_lastRecoilDir.x = 0.0f;
    __this->fields.m_lastRecoilDir.y = 0.0f;
    __this->fields.m_lastRecoilDir.z = 0.0f;
    __this->fields.currentRecoilForce.x = 0.0f;
    __this->fields.currentRecoilForce.y = 0.0f;
    __this->fields.currentRecoilForceVP.x = 0.0f;
    __this->fields.currentRecoilForceVP.y = 0.0f;
    __this->fields.m_recoilRotateOffset.x = 0.0f;
    __this->fields.m_recoilRotateOffset.y = 0.0f;
    __this->fields.m_recoilRotateOffset.z = 0.0f;
    __this->fields.m_centerVP.x = 0.0f;
    __this->fields.m_centerVP.y = 0.0f;
    __this->fields.CurrentVSPos.x = 0.0f;
    __this->fields.CurrentVSPos.y = 0.0f;
    __this->fields.m_FPS_SightOffset_Property.x = 0.0f;
    __this->fields.m_FPS_SightOffset_Property.y = 0.0f;
    __this->fields.m_FPS_SightOffset_Property.z = 0.0f;
    __this->fields.m_FPS_SightOffset_Property.w = 0.0f;
    __this->fields.m_tempWP.x = 0.0f;
    __this->fields.m_tempWP.y = 0.0f;
    __this->fields.m_tempWP.z = 0.0f;
    __this->fields.loops = 0.0f;
    __this->fields.m_autoInertiaDampen = 0.0f;
    __this->fields.m_autoInertiaSpring = 0.0f;
    __this->fields.m_autoInertiaStopTime = 0.0f;
    __this->fields.m_autoInertiaFadeDuration = 0.0f;

    __this->fields.m_returnVector.x = 0.0f;
    __this->fields.m_returnVector.y = 0.0f;


    recoilData->fields._spring_k__BackingField = 0.0f;
    recoilData->fields._dampening_k__BackingField = 0.0f;
    recoilData->fields._hipFireCrosshairSizeDefault_k__BackingField = 0.0f;
    recoilData->fields._hipFireCrosshairRecoilPop_k__BackingField = 0.0f;
    recoilData->fields._hipFireCrosshairSizeMax_k__BackingField = 0.0f;
    recoilData->fields._horizontalScale_k__BackingField->fields._Min_k__BackingField = 0.0f;
    recoilData->fields._horizontalScale_k__BackingField->fields._Max_k__BackingField = 0.0f;
    recoilData->fields._verticalScale_k__BackingField->fields._Min_k__BackingField = 0.0f;
    recoilData->fields._verticalScale_k__BackingField->fields._Max_k__BackingField = 0.0f;
    recoilData->fields._recoilPosImpulse_k__BackingField.x = 0.0f;
    recoilData->fields._recoilPosImpulse_k__BackingField.y = 0.0f;
    recoilData->fields._recoilPosImpulse_k__BackingField.z = 0.0f;
    recoilData->fields._recoilPosShiftWeight_k__BackingField = 0.0f;
    recoilData->fields._recoilPosStiffness_k__BackingField = 0.0f;
    recoilData->fields._recoilPosDamping_k__BackingField = 0.0f;
    recoilData->fields._recoilPosImpulseWeight_k__BackingField = 0.0f;
    recoilData->fields._recoilCameraPosWeight_k__BackingField = 0.0f;
    recoilData->fields._recoilAimingWeight_k__BackingField = 0.0f;
    recoilData->fields._recoilRotImpulse_k__BackingField.x = 0.0f;
    recoilData->fields._recoilRotImpulse_k__BackingField.y = 0.0f;
    recoilData->fields._recoilRotImpulse_k__BackingField.z = 0.0f;
    recoilData->fields._recoilRotStiffness_k__BackingField = 0.0f;
    recoilData->fields._recoilRotDamping_k__BackingField = 0.0f;
    recoilData->fields._recoilRotImpulseWeight_k__BackingField = 0.0f;
    recoilData->fields._recoilCameraRotWeight_k__BackingField = 0.0f;
    recoilData->fields._concussionIntensity_k__BackingField = 0.0f;
    recoilData->fields._concussionFrequency_k__BackingField = 0.0f;
    recoilData->fields._concussionDuration_k__BackingField = 0.0f;

    return __this->fields.m_returnVector;
}

bool Hooks::hkWeapon_CastWeaponRay(app::Transform* muzzle, app::Weapon_WeaponHitData** weaponRayData, int32_t altRayCastMask, MethodInfo* method)
{
    if (Player::noSpreadToggleKey.isToggled())
    {
        // This seems to only work on foam launcher
        (*weaponRayData)->fields.randomSpread = 0.0f;
    }
#ifdef USE_DETOURS
    return app::Weapon_CastWeaponRay(muzzle, weaponRayData, altRayCastMask, method);
#else
    static auto fpOFunc = reinterpret_cast<bool (*)(app::Transform*, app::Weapon_WeaponHitData**, int32_t, MethodInfo*)>(fpMap["Weapon_CastWeaponRay"]);
    return fpOFunc(muzzle, weaponRayData, altRayCastMask, method);
#endif // USE_DETOURS
}

bool Hooks::hkWeapon_CastWeaponRay_1(app::Transform* alignTransform, app::Weapon_WeaponHitData** weaponRayData, app::Vector3 originPos, int32_t altRayCastMask, MethodInfo* method)
{
    if (Player::noSpreadToggleKey.isToggled())
    {
        // Works on rest of guns
        (*weaponRayData)->fields.randomSpread = 0.0f;
        (*weaponRayData)->fields.angOffsetX = 0.0f;
        (*weaponRayData)->fields.angOffsetY = 0.0f;
    }
    if (Aimbot::settings.silentAim && Aimbot::isSilentAiming)
    {
        if (Aimbot::settings.magicBullet)
        {
            originPos = Aimbot::silentAimBone;
            originPos.y += 0.5f;
            (*weaponRayData)->fields.fireDir.x = 0.0f;
            (*weaponRayData)->fields.fireDir.y = -1.0f;
            (*weaponRayData)->fields.fireDir.z = 0.0f;
        }
        else
        {
            app::LocalPlayerAgent* localPlayerAgent = reinterpret_cast<app::LocalPlayerAgent*>(G::localPlayer);
            app::Vector3 boneVec = Math::Vector3Sub(Aimbot::silentAimBone, originPos);
            app::Quaternion quaternion = app::Quaternion_LookRotation(boneVec, app::Vector3_get_up(NULL), NULL);
            boneVec = app::Vector3_Normalize(boneVec, NULL);

            app::RaycastHit raycastHit; 
            if (app::Physics_Raycast_5(originPos, boneVec, &raycastHit, localPlayerAgent->fields.m_FPSCamera->fields.m_cameraRayDist, (*app::LayerManager__TypeInfo)->static_fields->MASK_CAMERA_RAY, NULL))
            {
                app::Vector3 newFireDir = Math::Vector3Sub(raycastHit.m_Point, originPos);
                newFireDir = app::Vector3_Normalize(newFireDir, NULL);
                (*weaponRayData)->fields.fireDir = newFireDir;
            }
        }
    }

#ifdef USE_DETOURS
    return app::Weapon_CastWeaponRay_1(alignTransform, weaponRayData, originPos, altRayCastMask, method);
#else
    static auto fpOFunc = reinterpret_cast<bool (*)(app::Transform*, app::Weapon_WeaponHitData**, app::Vector3, int32_t, MethodInfo*)>(fpMap["Weapon_CastWeaponRay_1"]);
    return fpOFunc(alignTransform, weaponRayData, originPos, altRayCastMask, method);
#endif // USE_DETOURS
}

float Hooks::hkPlayerAmmoStorage_UpdateBulletsInPack(app::PlayerAmmoStorage* __this, app::AmmoType__Enum ammoType, int32_t bulletCount, MethodInfo* method)
{
    // Works for glue gun, lock melter, mine deployer, resource pack, throw weapons
    if (Player::infiAmmoToggleKey.isToggled() && bulletCount < 0)
    {
        bulletCount = 0;
    }
#ifdef USE_DETOURS
    return app::PlayerAmmoStorage_UpdateBulletsInPack(__this, ammoType, bulletCount, method);
#else
    static auto fpOFunc = reinterpret_cast<float (*)(app::PlayerAmmoStorage*, app::AmmoType__Enum, int32_t, MethodInfo*)>(fpMap["PlayerAmmoStorage_UpdateBulletsInPack"]);
    return fpOFunc(__this, ammoType, bulletCount, method);
#endif // USE_DETOURS
}

void Hooks::hkBulletWeapon_Fire(app::BulletWeapon* __this, bool resetRecoilSimilarity, MethodInfo* method)
{
    // Works for normal bullet weapons
    if (Player::infiAmmoToggleKey.isToggled())
        __this->fields.m_clip++;

#ifdef USE_DETOURS
    return app::BulletWeapon_Fire(__this, resetRecoilSimilarity, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::BulletWeapon*, bool, MethodInfo*)>(fpMap["BulletWeapon_Fire"]);
    return fpOFunc(__this, resetRecoilSimilarity, method);
#endif // USE_DETOURS
}

void Hooks::hkShotgun_Fire(app::Shotgun* __this, bool resetRecoilSimilarity, MethodInfo* method)
{
    // Works for shotguns
    if (Player::infiAmmoToggleKey.isToggled())
        __this->fields._.m_clip++;

#ifdef USE_DETOURS
    return app::Shotgun_Fire(__this, resetRecoilSimilarity, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::Shotgun*, bool, MethodInfo*)>(fpMap["Shotgun_Fire"]);
    return fpOFunc(__this, resetRecoilSimilarity, method);
#endif // USE_DETOURS
}

void Hooks::hkGlueGun_Updatepressure(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method)
{
    if (Player::glueInstantToggleKey.isToggled())
        __this->fields.m_pressure = 1.0f;

#ifdef USE_DETOURS
    return app::GlueGun_Updatepressure(__this, syncData, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::GlueGun*, app::PlayerInventoryBase_pSimpleItemSyncData*, MethodInfo*)>(fpMap["GlueGun_Updatepressure"]);
    return fpOFunc(__this, syncData, method);
#endif // USE_DETOURS
}

void Hooks::hkGlueGun_UpdateRecharging(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method)
{
    if (Player::glueInstantToggleKey.isToggled())
        __this->fields.m_rechargeTimer = 0.0f;

#ifdef USE_DETOURS
    return app::GlueGun_UpdateRecharging(__this, syncData, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::GlueGun*, app::PlayerInventoryBase_pSimpleItemSyncData*, MethodInfo*)>(fpMap["GlueGun_UpdateRecharging"]);
    return fpOFunc(__this, syncData, method);
#endif // USE_DETOURS
}

void Hooks::hkHackingMinigame_TimingGrid_StartGame(app::HackingMinigame_TimingGrid* __this, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::HackingMinigame_TimingGrid_StartGame(__this, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::HackingMinigame_TimingGrid*, MethodInfo*)>(fpMap["HackingMinigame_TimingGrid_StartGame"]);
    fpOFunc(__this, method);
#endif // USE_DETOURS

    if (Player::instaHackToggleKey.isToggled())
        __this->fields.m_puzzleDone = true;
}

void Hooks::hkBulletWeaponArchetype_Update(app::BulletWeaponArchetype* __this, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::BulletWeaponArchetype_Update(__this, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::BulletWeaponArchetype*, MethodInfo*)>(fpMap["BulletWeaponArchetype_Update"]);
    fpOFunc(__this, method);
#endif // USE_DETOURS

    if (!Player::fullAutoToggleKey.isToggled())
        return;
    bool pressed = app::ItemEquippable_get_FireButton(reinterpret_cast<app::ItemEquippable*>(__this->fields.m_weapon), NULL);
    if (pressed)
        __this->fields.m_readyToFire = true;
    else
        __this->fields.m_readyToFire = false;
}

void Hooks::hkArtifactPickup_Core_Setup(app::ArtifactPickup_Core* __this, app::ArtifactCategory__Enum category, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::ArtifactPickup_Core_Setup(__this, category, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::ArtifactPickup_Core*, app::ArtifactCategory__Enum, MethodInfo*)>(fpMap["ArtifactPickup_Core_Setup"]);
    fpOFunc(__this, category, method);
#endif // USE_DETOURS

    G::worldArtifMtx.lock();
    ESP::worldArtifacts.push_back(ESP::WorldArtifactItem(__this));
    G::worldArtifMtx.unlock();
    return;
}

void Hooks::hkCommodityPickup_Core_Setup(app::CommodityPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    return app::CommodityPickup_Core_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::CommodityPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(fpMap["CommodityPickup_Core_Setup"]);
    return fpOFunc(__this, data, method);
#endif // USE_DETOURS
}

void Hooks::hkConsumablePickup_Core_Setup(app::ConsumablePickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::ConsumablePickup_Core_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::ConsumablePickup_Core*, app::ItemDataBlock*, MethodInfo*)>(fpMap["ConsumablePickup_Core_Setup"]);
    fpOFunc(__this, data, method);
#endif // USE_DETOURS

    auto pickupItem = reinterpret_cast<app::LG_PickupItem_Sync*>(__this->fields.m_sync);
    G::worldItemsMtx.lock();
    ESP::worldItems.push_back(ESP::WorldPickupItem(pickupItem));
    G::worldItemsMtx.unlock();
    return;
}

void Hooks::hkCarryItemPickup_Core_Setup(app::CarryItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::CarryItemPickup_Core_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::CarryItemPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(fpMap["CarryItemPickup_Core_Setup"]);
    fpOFunc(__this, data, method);
#endif // USE_DETOURS

    G::worldCarryMtx.lock();
    ESP::worldCarryItems.push_back(ESP::WorldCarryItem(__this));
    G::worldCarryMtx.unlock();
    return;
}

void Hooks::hkKeyItemPickup_Core_Setup(app::KeyItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::KeyItemPickup_Core_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::KeyItemPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(fpMap["KeyItemPickup_Core_Setup"]);
    fpOFunc(__this, data, method);
#endif // USE_DETOURS

    G::worldKeyMtx.lock();
    ESP::worldKeys.push_back(ESP::WorldKeyItem(__this));
    G::worldKeyMtx.unlock();
    return;
}

void Hooks::hkGenericSmallPickupItem_Core_Setup(app::GenericSmallPickupItem_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::GenericSmallPickupItem_Core_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::GenericSmallPickupItem_Core*, app::ItemDataBlock*, MethodInfo*)>(fpMap["GenericSmallPickupItem_Core_Setup"]);
    fpOFunc(__this, data, method);
#endif // USE_DETOURS

    G::worldGenericMtx.lock();
    ESP::worldGenerics.push_back(ESP::WorldGenericItem(__this));
    G::worldGenericMtx.unlock();
    return;
}

void Hooks::hkResourcePackPickup_Setup(app::ResourcePackPickup* __this, app::ItemDataBlock* data, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::ResourcePackPickup_Setup(__this, data, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::ResourcePackPickup*, app::ItemDataBlock*, MethodInfo*)>(fpMap["ResourcePackPickup_Setup"]);
    fpOFunc(__this, data, method);
#endif // USE_DETOURS

    G::worldResourcePackMtx.lock();
    ESP::worldResourcePacks.push_back(ESP::WorldResourceItem(__this));
    G::worldResourcePackMtx.unlock();
    return;
}

void Hooks::hkLG_HSU_Setup(app::LG_HSU* __this, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::LG_HSU_Setup(__this, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::LG_HSU*, MethodInfo*)>(fpMap["LG_HSU_Setup"]);
    fpOFunc(__this, method);
#endif // USE_DETOURS

    G::worldHSUMtx.lock();
    ESP::worldHSUItems.push_back(ESP::WorldHSUItem(__this));
    G::worldHSUMtx.unlock();
    return;
}


// For terminals
void Hooks::hkLG_FunctionMarkerBuilder_SetupFunctionGO(app::LG_FunctionMarkerBuilder* __this, app::LG_LayerType__Enum layer, app::GameObject* GO, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::LG_FunctionMarkerBuilder_SetupFunctionGO(__this, layer, GO, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::LG_FunctionMarkerBuilder*, app::LG_LayerType__Enum, app::GameObject*, MethodInfo*)>(fpMap["LG_FunctionMarkerBuilder_SetupFunctionGO"]);
    fpOFunc(__this, layer, GO, method);
#endif // USE_DETOURS

    auto terminalsSpawnedList = __this->fields.m_node->fields.m_zone->fields._TerminalsSpawnedInZone_k__BackingField;
    auto terminals = terminalsSpawnedList->fields._items->vector;
    
    G::worldTerminalsMtx.lock();
    
    for (int i = 0; i < terminalsSpawnedList->fields._size; i++)
        ESP::worldTerminals.push_back(ESP::WorldTerminalItem(terminals[i]));
    
    G::worldTerminalsMtx.unlock();

    return;
}

void Hooks::hkGameStateManager_ChangeState(app::eGameStateName__Enum nextState, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::GameStateManager_ChangeState(nextState, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::eGameStateName__Enum, MethodInfo*)>(fpMap["GameStateManager_ChangeState"]);
    fpOFunc(nextState, method);
#endif // USE_DETOURS

    if (nextState < app::eGameStateName__Enum::Generating || nextState > app::eGameStateName__Enum::InLevel)
    {
        G::mainCamera = NULL;

        G::worldItemsMtx.lock();
        ESP::worldItems.clear();
        G::worldItemsMtx.unlock();

        G::worldArtifMtx.lock();
        ESP::worldArtifacts.clear();
        G::worldArtifMtx.unlock();

        G::worldCarryMtx.lock();
        ESP::worldCarryItems.clear();
        G::worldCarryMtx.unlock();
        
        G::worldKeyMtx.lock();
        ESP::worldKeys.clear();
        G::worldKeyMtx.unlock();
        
        G::worldGenericMtx.lock();
        ESP::worldGenerics.clear();
        G::worldGenericMtx.unlock();
        
        G::worldResourcePackMtx.lock();
        ESP::worldResourcePacks.clear();
        G::worldResourcePackMtx.unlock();
        
        G::worldTerminalsMtx.lock();
        ESP::worldTerminals.clear();
        G::worldTerminalsMtx.unlock();

        G::worldHSUMtx.lock();
        ESP::worldHSUItems.clear();
        G::worldHSUMtx.unlock();
    }
}

// Debug hook to get list of possible items
void Hooks::hkItemDataBlock_OnPostSetup(app::ItemDataBlock* __this, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::ItemDataBlock_OnPostSetup(__this, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::ItemDataBlock*, MethodInfo*)>(fpMap["ItemDataBlock_OnPostSetup"]);
    fpOFunc(__this, method);
#endif // USE_DETOURS

    auto betterString = il2cppi_to_string(__this->fields._publicName_k__BackingField);
    auto termName = il2cppi_to_string(__this->fields._terminalItemShortName_k__BackingField);
    std::string itemStr = betterString + "    |    " + termName;
    il2cppi_log_write(itemStr);
}

void Hooks::hkCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method)
{
    if (G::showMenu)
        value = app::CursorLockMode__Enum::Confined;

#ifdef USE_DETOURS
    app::Cursor_set_lockState(value, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::CursorLockMode__Enum, MethodInfo*)>(fpMap["Cursor_set_lockState"]);
    fpOFunc(value, method);
#endif // USE_DETOURS
}

void Hooks::hkCursor_set_visible(bool value, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::Cursor_set_visible(G::showMenu, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(bool, MethodInfo*)>(fpMap["Cursor_set_visible"]);
    fpOFunc(G::showMenu, method);
#endif // USE_DETOURS
}

void Hooks::hkLocalPlayerAgent_Update(app::LocalPlayerAgent* __this, MethodInfo* method)
{
#ifdef USE_DETOURS
    app::LocalPlayerAgent_Update(__this, method);
#else
    static auto fpOFunc = reinterpret_cast<void (*)(app::LocalPlayerAgent*, MethodInfo*)>(fpMap["LocalPlayerAgent_Update"]);
    fpOFunc(__this, method);
#endif // USE_DETOURS

    Enemy::_RefreshEnemyAgents();
    while (!G::callbacks.empty())
    {
        G::callbacks.front()();
        G::callbacks.pop();
    }
    
    G::w2CamMatrix = app::Camera_get_worldToCameraMatrix(G::mainCamera, NULL);
    G::projMatrix = app::Camera_get_projectionMatrix(G::mainCamera, NULL);
    G::viewMatrix = Math::MatrixMult(G::projMatrix, G::w2CamMatrix);

    G::worldItemsMtx.lock();
    for (ESP::WorldPickupItem& i : ESP::worldItems) i.update();
    std::sort(ESP::worldItems.begin(), ESP::worldItems.end(), std::greater<ESP::WorldPickupItem>());
    G::worldItemsMtx.unlock();

    G::worldArtifMtx.lock();
    for (ESP::WorldArtifactItem& i : ESP::worldArtifacts) i.update();
    std::sort(ESP::worldArtifacts.begin(), ESP::worldArtifacts.end(), std::greater<ESP::WorldArtifactItem>());
    G::worldArtifMtx.unlock();

    G::worldCarryMtx.lock();
    for (ESP::WorldCarryItem& i : ESP::worldCarryItems) i.update();
    std::sort(ESP::worldCarryItems.begin(), ESP::worldCarryItems.end(), std::greater<ESP::WorldCarryItem>());
    G::worldCarryMtx.unlock();
    
    G::worldKeyMtx.lock();
    for (ESP::WorldKeyItem& i : ESP::worldKeys) i.update();
    std::sort(ESP::worldKeys.begin(), ESP::worldKeys.end(), std::greater<ESP::WorldKeyItem>());
    G::worldKeyMtx.unlock();
    
    G::worldGenericMtx.lock();
    for (ESP::WorldGenericItem& i : ESP::worldGenerics) i.update();
    std::sort(ESP::worldGenerics.begin(), ESP::worldGenerics.end(), std::greater<ESP::WorldGenericItem>());
    G::worldGenericMtx.unlock();
    
    G::worldResourcePackMtx.lock();
    for (ESP::WorldResourceItem& i : ESP::worldResourcePacks) i.update();
    std::sort(ESP::worldResourcePacks.begin(), ESP::worldResourcePacks.end(), std::greater<ESP::WorldResourceItem>());
    G::worldResourcePackMtx.unlock();
    
    G::worldTerminalsMtx.lock();
    for (ESP::WorldTerminalItem& i : ESP::worldTerminals) i.update();
    std::sort(ESP::worldTerminals.begin(), ESP::worldTerminals.end(), std::greater<ESP::WorldTerminalItem>());
    G::worldTerminalsMtx.unlock();

    G::worldHSUMtx.lock();
    for (ESP::WorldHSUItem& i : ESP::worldHSUItems) i.update();
    std::sort(ESP::worldHSUItems.begin(), ESP::worldHSUItems.end(), std::greater<ESP::WorldHSUItem>());
    G::worldHSUMtx.unlock();

    Aimbot::RunAimbot();
}

bool Hooks::hkDam_EnemyDamageBase_ProcessReceivedDamage(app::Dam_EnemyDamageBase* __this, float damage, app::Agent* damageSource,
    app::Vector3 position, app::Vector3 direction, app::ES_HitreactType__Enum hitreact, bool tryForceHitreact, int32_t limbID,
    float staggerDamageMulti, app::DamageNoiseLevel__Enum damageNoiseLevel,
    uint32_t gearCategoryId, MethodInfo* method)
{
#ifdef USE_DETOURS
    bool retVal = app::Dam_EnemyDamageBase_ProcessReceivedDamage(__this, damage, damageSource, position, direction, hitreact, tryForceHitreact, limbID, staggerDamageMulti, damageNoiseLevel, gearCategoryId, method);
#else
    static auto fpOFunc = reinterpret_cast<bool (*)(app::Dam_EnemyDamageBase*, float, app::Agent*, app::Vector3, app::Vector3, app::ES_HitreactType__Enum, bool, int32_t, float, app::DamageNoiseLevel__Enum, uint32_t, MethodInfo*)>(fpMap["Dam_EnemyDamageBase_ProcessReceivedDamage"]);
    bool retVal = fpOFunc(__this, damage, damageSource, position, direction, hitreact, tryForceHitreact, limbID, staggerDamageMulti, damageNoiseLevel, gearCategoryId, method);
#endif // USE_DETOURS

    if ((*app::SNet__TypeInfo)->static_fields->_IsMaster_k__BackingField)
    {
        auto syncedDmg = reinterpret_cast<app::Dam_SyncedDamageBase*>(__this);
        //std::cout << "Trying to set health to: " << std::to_string(syncedDmg->fields._Health_k__BackingField) << std::endl;
        app::Dam_SyncedDamageBase_SendSetHealth(syncedDmg, syncedDmg->fields._Health_k__BackingField, NULL);
    }

    return retVal;
}

LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (G::imguiMtx.try_lock())
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        G::imguiMtx.unlock();
    }
    if (G::showMenu)
        return TRUE;
    
    return CallWindowProc(G::oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!G::initialized)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&G::pDevice)))
        {
            ImGui::CreateContext();

            G::pDevice->GetImmediateContext(&G::pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            G::windowHwnd = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = NULL;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            G::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &G::mainRenderTargetView);
            pBackBuffer->Release();

            ImGuiIO& io = ImGui::GetIO();
            io.ImeWindowHandle = G::windowHwnd;
            G::defaultFont = io.Fonts->AddFontDefault();
            ImFontConfig config;
            config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
            G::espFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(Fonts::GetRobotoFontDataTTFBase85(), 14, &config);
            unsigned char* pixels;
            int width, height;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            ImGui_ImplWin32_Init(G::windowHwnd);
            ImGui_ImplDX11_Init(G::pDevice, G::pContext);
            ImGui_ImplDX11_CreateDeviceObjects();            
            
            G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
            G::initialized = true;    
        }
        else
        {
            return G::oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    G::imguiMtx.lock();
    ImGui::NewFrame();
    G::imguiMtx.unlock();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f)
    {
        RenderWatermark();
        RenderESP();


        if (G::menuKey.isPressed())
            G::showMenu = !G::showMenu;
        if (G::unloadKey.isPressed())
            G::running = false;

        Player::UpdateInput();
        ESP::UpdateInput();

        if (G::showMenu)
            DrawMenu();
    }


    ImGui::EndFrame();
    ImGui::Render();

    G::pContext->OMSetRenderTargets(1, &G::mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return G::oPresent(pSwapChain, SyncInterval, Flags);
}