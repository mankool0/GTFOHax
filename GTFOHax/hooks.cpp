#include "hooks.h"
#include "menu.h"
#include "hacks/player.h"
#include "framework/helpers.h"
#include "hacks/esp.h"
#include "fonts/fonts.h"
#include "hacks/enemy.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <misc/freetype/imgui_freetype.h>
#include "utils/math.h"
#include "hacks/aimbot.h"
#include "i18n/i18n.h"

#include "MinHook.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool hookFailed = false;
std::map<std::string, LPVOID> hooks;
std::vector<std::pair<LPVOID, std::string>> hookTargets;

std::string HookErrorToString(LONG code)
{
    switch (code)
    {
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
    default:
        return std::to_string(code);
    }
}

void HookAttach(PVOID ppPointer, PVOID pDetour, PVOID* fpOrig, std::string functionName)
{
    MH_STATUS codeCreate, codeEnable = MH_UNKNOWN;
    codeCreate = MH_CreateHook(ppPointer, pDetour, fpOrig);
    if (codeCreate == MH_OK)
    {
        hooks[functionName] = *fpOrig;
        hookTargets.push_back({ppPointer, functionName});
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

static float s_pendingBulletDamage = 0.0f;

#define HOOKATTACH(fun) void (*fp ## fun)(void); \
                        HookAttach(reinterpret_cast<PVOID>(app::fun), reinterpret_cast<PVOID>(&Hooks::hk ## fun), reinterpret_cast<PVOID*>(&fp ## fun), #fun)
#define HOOKDETACH(fun) (HookDetach(reinterpret_cast<PVOID>(app::fun), #fun))

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
        if (blockName == "Striker_Boss" || blockName == "Cocoon"
            || blockName == "Squidward" || blockName == "SquidBoss_Big" 
            || blockName == "SquidBoss Big Complex" || blockName == "SquidBoss VS")
        {
            continue; //Ignore enemies that can't be spawned
        }
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

    il2cppi_log_write("Using MinHook for hooking");

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
    HOOKATTACH(LG_BulkheadDoorController_Core_Setup);
    HOOKATTACH(LG_ComputerTerminal_Setup);

    HOOKATTACH(GameStateManager_ChangeState);
    HOOKATTACH(Application_Quit);
    HOOKATTACH(Application_Quit_1);

    HOOKATTACH(Cursor_set_lockState);
    HOOKATTACH(Cursor_set_visible);

    HOOKATTACH(LocalPlayerAgent_Update);
    HOOKATTACH(LocalPlayerAgent_LateUpdate);

    HOOKATTACH(Dam_EnemyDamageBase_ProcessReceivedDamage);
    HOOKATTACH(ArchetypeDataBlock_GetSentryDamage);
    HOOKATTACH(Dam_EnemyDamageLimb_ApplyWeakspotAndArmorModifiers);
    HOOKATTACH(Dam_EnemyDamageLimb_Custom_ApplyWeakspotAndArmorModifiers);

    HOOKATTACH(PreLitVolume_Update);
    HOOKATTACH(RenderPipe_CameraUpdate);

    HOOKATTACH(PLOC_Downed_Enter);
    HOOKATTACH(Dam_PlayerDamageLocal_OnRevive);

    if (hookFailed)
        il2cppi_log_write("Failed Initializing Hooks");
    else
        il2cppi_log_write("Initialized Hooks");
}

void Hooks::RemoveHooks(bool fullCleanup)
{
    static bool isRemoved = false;
    if (isRemoved) return;
    isRemoved = true;

    G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (LONG_PTR)G::oWndProc);
    
    for (auto const& pair : hookTargets)
    {
        LPVOID target = pair.first;
        std::string functionName = pair.second;
        MH_STATUS remove_status = MH_RemoveHook(target);
        if (remove_status != MH_OK) {
            std::string error = "Hook removal failed for " + functionName + ", error: " + HookErrorToString(remove_status);
            il2cppi_log_write(error);
        }
    }
    
    if (fullCleanup)
    {
        kiero::shutdown();

        if (ImGui::GetCurrentContext()) {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        MH_Uninitialize();
    }
    else
    {
        MH_DisableHook(MH_ALL_HOOKS);
    }

    hookTargets.clear();
    hooks.clear();

    il2cppi_log_write("Detached Hooks");
}

bool Hooks::hkDam_PlayerDamageBase_OnIncomingDamage(app::Dam_PlayerDamageBase* __this, float damage, float originalDamage, app::Agent* source, MethodInfo* method)
{
    if (!Player::godmodeToggleKey.isToggled())
    {
        auto agent = app::Dam_PlayerDamageBase_GetBaseAgent(__this, nullptr);
        bool isLocal = (agent == reinterpret_cast<app::Agent*>(G::localPlayer));
        if (isLocal && Player::localDamageToggleKey.isToggled())
        {
            damage *= Player::localDamageMulti;
            originalDamage *= Player::localDamageMulti;
        }
        else if (!isLocal && Player::teamDamageToggleKey.isToggled())
        {
            damage *= Player::teamDamageMulti;
            originalDamage *= Player::teamDamageMulti;
        }
        static auto fpOFunc = reinterpret_cast<bool(*)(app::Dam_PlayerDamageBase*, float, float, app::Agent*, MethodInfo*)>(hooks["Dam_PlayerDamageBase_OnIncomingDamage"]);
        return fpOFunc(__this, damage, originalDamage, source, method);
    }

    return false;
}

void Hooks::hkPLOC_Downed_Enter(app::PLOC_Downed* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::PLOC_Downed*, MethodInfo*)>(hooks["PLOC_Downed_Enter"]);
    fpOFunc(__this, method);

    if (!Player::autoSelfReviveToggleKey.isToggled()) return;

    auto owner = __this->fields._.m_owner;
    auto localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
    if (!owner || owner != localPlayer) return;

    app::Vector3 pos = localPlayer->fields._.m_position;
    app::AgentReplicatedActions_PlayerReviveAction(localPlayer, localPlayer, pos, nullptr);
}

void Hooks::hkDam_PlayerDamageLocal_OnRevive(app::Dam_PlayerDamageLocal* __this, app::pSetHealthData data, MethodInfo* method)
{
    if (Player::fullHpReviveToggleKey.isToggled())
        data.health.internalValue = 0xFFFF;

    static auto fpOFunc = reinterpret_cast<void (*)(app::Dam_PlayerDamageLocal*, app::pSetHealthData, MethodInfo*)>(hooks["Dam_PlayerDamageLocal_OnRevive"]);
    fpOFunc(__this, data, method);
}

void Hooks::hkDam_PlayerDamageBase_ModifyInfection(app::Dam_PlayerDamageBase* __this, app::pInfection data, bool sync, bool updatePageMap, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::Dam_PlayerDamageBase*, app::pInfection, bool, bool, MethodInfo*)>(hooks["Dam_PlayerDamageBase_ModifyInfection"]);
    fpOFunc(__this, data, sync, updatePageMap, method);
    if (Player::godmodeToggleKey.isToggled())
        __this->fields.Infection = 0.0f;
}

void Hooks::hkDam_PlayerDamageLocal_Hitreact(app::Dam_PlayerDamageLocal* __this, float damage, app::Vector3 direction, bool triggerCameraShake, bool triggerGenericDialog, bool pushPlayer, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::Dam_PlayerDamageLocal*, float, app::Vector3, bool, bool, bool, MethodInfo*)>(hooks["Dam_PlayerDamageLocal_Hitreact"]);
    fpOFunc(__this, damage, direction, !Player::noShakeToggleKey.isToggled(), triggerGenericDialog, pushPlayer, method);
}

app::Vector2 Hooks::hkFPS_RecoilSystem_ApplyRecoil(app::FPS_RecoilSystem* __this, bool resetSimilarity, app::RecoilDataBlock* recoilData, MethodInfo* method)
{
    if (!Player::noRecoilToggleKey.isToggled())
    {
        static auto fpOFunc = reinterpret_cast<app::Vector2(*)(app::FPS_RecoilSystem*, bool, app::RecoilDataBlock*, MethodInfo*)>(hooks["FPS_RecoilSystem_ApplyRecoil"]);
        return fpOFunc(__this, resetSimilarity, recoilData, method);
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
    static auto fpOFunc = reinterpret_cast<bool (*)(app::Transform*, app::Weapon_WeaponHitData**, int32_t, MethodInfo*)>(hooks["Weapon_CastWeaponRay"]);
    return fpOFunc(muzzle, weaponRayData, altRayCastMask, method);
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
            // Get the real-time position of the target to fix miss on fast-moving enemies
            app::Vector3 currentTargetPos = Aimbot::GetCurrentTargetPosition();
            float offset = Aimbot::settings.magicBulletOffset;
            
            // Calculate origin position and fire direction based on selected direction mode
            switch (Aimbot::settings.magicBulletDirection)
            {
                case Aimbot::MagicBulletDirection::FromAbove:
                default:
                {
                    // Default: shoot from above the target, downward
                    originPos = currentTargetPos;
                    originPos.y += offset;
                    (*weaponRayData)->fields.fireDir.x = 0.0f;
                    (*weaponRayData)->fields.fireDir.y = -1.0f;
                    (*weaponRayData)->fields.fireDir.z = 0.0f;
                    break;
                }
                case Aimbot::MagicBulletDirection::FromFront:
                {
                    // Shoot from enemy's front (based on movement direction, fallback to facing direction)
                    bool directionSet = false;
                    {
                        // Thread-safe access to targetEnemy
                        std::lock_guard<std::mutex> lock(G::enemyAimMtx);
                        app::EnemyAgent* localTargetEnemy = Aimbot::targetEnemy;
                        
                        // Validate pointer is still in the enemy list
                        bool isValid = false;
                        if (localTargetEnemy != nullptr)
                        {
                            auto aimSnap = Enemy::enemiesReady.load();
                            if (aimSnap)
                                for (const auto& enemyInfo : *aimSnap)
                                    if (enemyInfo->enemyAgent == localTargetEnemy) { isValid = true; break; }
                        }
                        
                        if (isValid)
                        {
                            // Try to get movement direction first
                            app::Vector3 enemyDir = Enemy::GetEnemyMovementDirection(localTargetEnemy);
                            bool hasMovementDir = (enemyDir.x != 0.0f || enemyDir.z != 0.0f);
                            
                            // Fallback to facing direction if not moving
                            if (!hasMovementDir)
                            {
                                app::Transform* enemyTransform = app::Component_1_get_transform(
                                    reinterpret_cast<app::Component_1*>(localTargetEnemy), NULL);
                                if (enemyTransform != nullptr)
                                {
                                    enemyDir = app::Transform_get_forward(enemyTransform, NULL);
                                    float len = sqrtf(enemyDir.x * enemyDir.x + enemyDir.z * enemyDir.z);
                                    if (len > 0.001f)
                                    {
                                        enemyDir.x /= len;
                                        enemyDir.z /= len;
                                        hasMovementDir = true;
                                    }
                                }
                            }
                            
                            if (hasMovementDir)
                            {
                                // Origin is in front of enemy (where enemy is moving/facing)
                                originPos.x = currentTargetPos.x + enemyDir.x * offset;
                                originPos.y = currentTargetPos.y;
                                originPos.z = currentTargetPos.z + enemyDir.z * offset;
                                // Fire direction is opposite (shooting into the enemy's face)
                                (*weaponRayData)->fields.fireDir.x = -enemyDir.x;
                                (*weaponRayData)->fields.fireDir.y = 0.0f;
                                (*weaponRayData)->fields.fireDir.z = -enemyDir.z;
                                directionSet = true;
                            }
                        }
                    }
                    // Fallback to FromAbove if targetEnemy is null or direction couldn't be determined
                    if (!directionSet)
                    {
                        originPos = currentTargetPos;
                        originPos.y += offset;
                        (*weaponRayData)->fields.fireDir.x = 0.0f;
                        (*weaponRayData)->fields.fireDir.y = -1.0f;
                        (*weaponRayData)->fields.fireDir.z = 0.0f;
                    }
                    break;
                }
                case Aimbot::MagicBulletDirection::FromBehind:
                {
                    // Shoot from enemy's back (opposite to movement direction, fallback to facing direction)
                    bool directionSet = false;
                    {
                        // Thread-safe access to targetEnemy
                        std::lock_guard<std::mutex> lock(G::enemyAimMtx);
                        app::EnemyAgent* localTargetEnemy = Aimbot::targetEnemy;
                        
                        // Validate pointer is still in the enemy list
                        bool isValid = false;
                        if (localTargetEnemy != nullptr)
                        {
                            auto aimSnap = Enemy::enemiesReady.load();
                            if (aimSnap)
                                for (const auto& enemyInfo : *aimSnap)
                                    if (enemyInfo->enemyAgent == localTargetEnemy) { isValid = true; break; }
                        }
                        
                        if (isValid)
                        {
                            // Try to get movement direction first
                            app::Vector3 enemyDir = Enemy::GetEnemyMovementDirection(localTargetEnemy);
                            bool hasMovementDir = (enemyDir.x != 0.0f || enemyDir.z != 0.0f);
                            
                            // Fallback to facing direction if not moving
                            if (!hasMovementDir)
                            {
                                app::Transform* enemyTransform = app::Component_1_get_transform(
                                    reinterpret_cast<app::Component_1*>(localTargetEnemy), NULL);
                                if (enemyTransform != nullptr)
                                {
                                    enemyDir = app::Transform_get_forward(enemyTransform, NULL);
                                    float len = sqrtf(enemyDir.x * enemyDir.x + enemyDir.z * enemyDir.z);
                                    if (len > 0.001f)
                                    {
                                        enemyDir.x /= len;
                                        enemyDir.z /= len;
                                        hasMovementDir = true;
                                    }
                                }
                            }
                            
                            if (hasMovementDir)
                            {
                                // Origin is behind enemy (opposite to where enemy is moving/facing)
                                originPos.x = currentTargetPos.x - enemyDir.x * offset;
                                originPos.y = currentTargetPos.y;
                                originPos.z = currentTargetPos.z - enemyDir.z * offset;
                                // Fire direction is same as enemy's direction (shooting into enemy's back)
                                (*weaponRayData)->fields.fireDir.x = enemyDir.x;
                                (*weaponRayData)->fields.fireDir.y = 0.0f;
                                (*weaponRayData)->fields.fireDir.z = enemyDir.z;
                                directionSet = true;
                            }
                        }
                    }
                    // Fallback to FromAbove if targetEnemy is null or direction couldn't be determined
                    if (!directionSet)
                    {
                        originPos = currentTargetPos;
                        originPos.y += offset;
                        (*weaponRayData)->fields.fireDir.x = 0.0f;
                        (*weaponRayData)->fields.fireDir.y = -1.0f;
                        (*weaponRayData)->fields.fireDir.z = 0.0f;
                    }
                    break;
                }
                case Aimbot::MagicBulletDirection::FromPlayer:
                {
                    // Shoot from player's direction toward enemy
                    bool directionSet = false;
                    if (G::localPlayer != nullptr)
                    {
                        app::Vector3 playerEyePos = G::localPlayer->fields.m_eyePosition;
                        app::Vector3 dirToEnemy = Math::Vector3Sub(currentTargetPos, playerEyePos);
                        float len = sqrtf(dirToEnemy.x * dirToEnemy.x + dirToEnemy.y * dirToEnemy.y + dirToEnemy.z * dirToEnemy.z);
                        if (len > 0.001f)
                        {
                            dirToEnemy = app::Vector3_Normalize(dirToEnemy, NULL);
                            // Origin is offset distance away from target, toward player
                            originPos.x = currentTargetPos.x - dirToEnemy.x * offset;
                            originPos.y = currentTargetPos.y - dirToEnemy.y * offset;
                            originPos.z = currentTargetPos.z - dirToEnemy.z * offset;
                            // Fire direction is toward the enemy
                            (*weaponRayData)->fields.fireDir = dirToEnemy;
                            directionSet = true;
                        }
                    }
                    // Fallback to FromAbove if localPlayer is null or direction couldn't be determined
                    if (!directionSet)
                    {
                        originPos = currentTargetPos;
                        originPos.y += offset;
                        (*weaponRayData)->fields.fireDir.x = 0.0f;
                        (*weaponRayData)->fields.fireDir.y = -1.0f;
                        (*weaponRayData)->fields.fireDir.z = 0.0f;
                    }
                    break;
                }
            }
            
            // Record bullet ray using actual shooting parameters
            if (Aimbot::settings.bulletRayEnabled)
            {
                // Use actual fireDir to calculate ray endpoint
                app::Vector3 fireDir = (*weaponRayData)->fields.fireDir;
                float rayLength = offset * 2.0f;  // Length of visible ray (covers the offset distance)
                app::Vector3 rayEnd;
                rayEnd.x = originPos.x + fireDir.x * rayLength;
                rayEnd.y = originPos.y + fireDir.y * rayLength;
                rayEnd.z = originPos.z + fireDir.z * rayLength;
                Aimbot::AddBulletRay(originPos, rayEnd);
            }
            
            // Record hit ghost effect - capture current skeleton positions
            if (Aimbot::settings.hitGhostEnabled)
            {
                std::lock_guard<std::mutex> lock(G::enemyAimMtx);
                app::EnemyAgent* localTargetEnemy = Aimbot::targetEnemy;
                
                if (localTargetEnemy != nullptr)
                {
                    auto aimSnap = Enemy::enemiesReady.load();
                    for (const auto& enemyInfo : (aimSnap ? *aimSnap : Enemy::EnemyVec{}))
                    {
                        if (enemyInfo->enemyAgent == localTargetEnemy)
                        {
                            // Get real-time skeleton positions
                            std::map<app::HumanBodyBones__Enum, Enemy::Bone> currentBones;
                            for (const auto& boneType : Enemy::WantedBones)
                            {
                                const Enemy::Bone* b = enemyInfo->getBone(boneType);
                                if (b)
                                {
                                    app::Transform* boneTransform = app::Animator_GetBoneTransform(
                                        localTargetEnemy->fields.Anim, boneType, NULL);
                                    if (boneTransform != nullptr)
                                    {
                                        Enemy::Bone bone = *b;
                                        app::Transform_get_position_Injected(boneTransform, &bone.position, NULL);
                                        currentBones[boneType] = bone;
                                    }
                                }
                            }
                            if (!currentBones.empty())
                            {
                                Aimbot::AddHitGhost(currentBones);
                            }
                            break;
                        }
                    }
                }
            }
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

    static auto fpOFunc = reinterpret_cast<bool (*)(app::Transform*, app::Weapon_WeaponHitData**, app::Vector3, int32_t, MethodInfo*)>(hooks["Weapon_CastWeaponRay_1"]);
    return fpOFunc(alignTransform, weaponRayData, originPos, altRayCastMask, method);
}

float Hooks::hkPlayerAmmoStorage_UpdateBulletsInPack(app::PlayerAmmoStorage* __this, app::AmmoType__Enum ammoType, int32_t bulletCount, MethodInfo* method)
{
    // Works for glue gun, lock melter, mine deployer, resource pack, throw weapons
    if (Player::infiAmmoToggleKey.isToggled() && bulletCount < 0)
    {
        bulletCount = 0;
    }
    static auto fpOFunc = reinterpret_cast<float (*)(app::PlayerAmmoStorage*, app::AmmoType__Enum, int32_t, MethodInfo*)>(hooks["PlayerAmmoStorage_UpdateBulletsInPack"]);
    return fpOFunc(__this, ammoType, bulletCount, method);
}

void Hooks::hkBulletWeapon_Fire(app::BulletWeapon* __this, bool resetRecoilSimilarity, MethodInfo* method)
{
    // Works for normal bullet weapons
    if (Player::infiAmmoToggleKey.isToggled())
        __this->fields.m_clip++;

    static auto fpOFunc = reinterpret_cast<void (*)(app::BulletWeapon*, bool, MethodInfo*)>(hooks["BulletWeapon_Fire"]);
    return fpOFunc(__this, resetRecoilSimilarity, method);
}

void Hooks::hkShotgun_Fire(app::Shotgun* __this, bool resetRecoilSimilarity, MethodInfo* method)
{
    // Works for shotguns
    if (Player::infiAmmoToggleKey.isToggled())
        __this->fields._.m_clip++;

    static auto fpOFunc = reinterpret_cast<void (*)(app::Shotgun*, bool, MethodInfo*)>(hooks["Shotgun_Fire"]);
    return fpOFunc(__this, resetRecoilSimilarity, method);
}

void Hooks::hkGlueGun_Updatepressure(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method)
{
    if (Player::glueInstantToggleKey.isToggled())
        __this->fields.m_pressure = 1.0f;

    static auto fpOFunc = reinterpret_cast<void (*)(app::GlueGun*, app::PlayerInventoryBase_pSimpleItemSyncData*, MethodInfo*)>(hooks["GlueGun_Updatepressure"]);
    return fpOFunc(__this, syncData, method);
}

void Hooks::hkGlueGun_UpdateRecharging(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method)
{
    if (Player::glueInstantToggleKey.isToggled())
        __this->fields.m_rechargeTimer = 0.0f;

    static auto fpOFunc = reinterpret_cast<void (*)(app::GlueGun*, app::PlayerInventoryBase_pSimpleItemSyncData*, MethodInfo*)>(hooks["GlueGun_UpdateRecharging"]);
    return fpOFunc(__this, syncData, method);
}

void Hooks::hkHackingMinigame_TimingGrid_StartGame(app::HackingMinigame_TimingGrid* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::HackingMinigame_TimingGrid*, MethodInfo*)>(hooks["HackingMinigame_TimingGrid_StartGame"]);
    fpOFunc(__this, method);

    if (Player::instaHackToggleKey.isToggled())
        __this->fields.m_puzzleDone = true;
}

void Hooks::hkBulletWeaponArchetype_Update(app::BulletWeaponArchetype* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::BulletWeaponArchetype*, MethodInfo*)>(hooks["BulletWeaponArchetype_Update"]);
    fpOFunc(__this, method);

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
    static auto fpOFunc = reinterpret_cast<void (*)(app::ArtifactPickup_Core*, app::ArtifactCategory__Enum, MethodInfo*)>(hooks["ArtifactPickup_Core_Setup"]);
    fpOFunc(__this, category, method);

    G::worldArtifMtx.lock();
    ESP::worldArtifacts.push_back(ESP::WorldArtifactItem(__this));
    G::worldArtifMtx.unlock();
    return;
}

void Hooks::hkCommodityPickup_Core_Setup(app::CommodityPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::CommodityPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(hooks["CommodityPickup_Core_Setup"]);
    return fpOFunc(__this, data, method);
}

void Hooks::hkConsumablePickup_Core_Setup(app::ConsumablePickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::ConsumablePickup_Core*, app::ItemDataBlock*, MethodInfo*)>(hooks["ConsumablePickup_Core_Setup"]);
    fpOFunc(__this, data, method);

    auto pickupItem = reinterpret_cast<app::LG_PickupItem_Sync*>(__this->fields.m_sync);
    G::worldItemsMtx.lock();
    ESP::worldItems.push_back(ESP::WorldPickupItem(pickupItem));
    G::worldItemsMtx.unlock();
    return;
}

void Hooks::hkCarryItemPickup_Core_Setup(app::CarryItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::CarryItemPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(hooks["CarryItemPickup_Core_Setup"]);
    fpOFunc(__this, data, method);

    G::worldCarryMtx.lock();
    ESP::worldCarryItems.push_back(ESP::WorldCarryItem(__this));
    G::worldCarryMtx.unlock();
    return;
}

void Hooks::hkKeyItemPickup_Core_Setup(app::KeyItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::KeyItemPickup_Core*, app::ItemDataBlock*, MethodInfo*)>(hooks["KeyItemPickup_Core_Setup"]);
    fpOFunc(__this, data, method);

    G::worldKeyMtx.lock();
    ESP::worldKeys.push_back(ESP::WorldKeyItem(__this));
    G::worldKeyMtx.unlock();
    return;
}

void Hooks::hkGenericSmallPickupItem_Core_Setup(app::GenericSmallPickupItem_Core* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::GenericSmallPickupItem_Core*, app::ItemDataBlock*, MethodInfo*)>(hooks["GenericSmallPickupItem_Core_Setup"]);
    fpOFunc(__this, data, method);

    G::worldGenericMtx.lock();
    ESP::worldGenerics.push_back(ESP::WorldGenericItem(__this));
    G::worldGenericMtx.unlock();
    return;
}

void Hooks::hkResourcePackPickup_Setup(app::ResourcePackPickup* __this, app::ItemDataBlock* data, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::ResourcePackPickup*, app::ItemDataBlock*, MethodInfo*)>(hooks["ResourcePackPickup_Setup"]);
    fpOFunc(__this, data, method);

    G::worldResourcePackMtx.lock();
    ESP::worldResourcePacks.push_back(ESP::WorldResourceItem(__this));
    G::worldResourcePackMtx.unlock();
    return;
}

void Hooks::hkLG_HSU_Setup(app::LG_HSU* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::LG_HSU*, MethodInfo*)>(hooks["LG_HSU_Setup"]);
    fpOFunc(__this, method);

    G::worldHSUMtx.lock();
    ESP::worldHSUItems.push_back(ESP::WorldHSUItem(__this));
    G::worldHSUMtx.unlock();
    return;
}

void Hooks::hkLG_BulkheadDoorController_Core_Setup(app::LG_BulkheadDoorController_Core* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::LG_BulkheadDoorController_Core*, MethodInfo*)>(hooks["LG_BulkheadDoorController_Core_Setup"]);
    fpOFunc(__this, method);

    G::worldBulkheadMtx.lock();
    ESP::worldBulkheadDCs.push_back(ESP::WorldBulkheadDC(__this));
    G::worldBulkheadMtx.unlock();
}

// For terminals
void Hooks::hkLG_ComputerTerminal_Setup(app::LG_ComputerTerminal* __this, app::TerminalStartStateData* startStateData, app::TerminalPlacementData* terminalPlacementData, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::LG_ComputerTerminal*, app::TerminalStartStateData*, app::TerminalPlacementData*, MethodInfo*)>(hooks["LG_ComputerTerminal_Setup"]);
    fpOFunc(__this, startStateData, terminalPlacementData, method);

    G::worldTerminalsMtx.lock();
    ESP::worldTerminals.push_back(ESP::WorldTerminalItem(__this));
    G::worldTerminalsMtx.unlock();
}

void Hooks::hkGameStateManager_ChangeState(app::eGameStateName__Enum nextState, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::eGameStateName__Enum, MethodInfo*)>(hooks["GameStateManager_ChangeState"]);
    fpOFunc(nextState, method);

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

        G::worldBulkheadMtx.lock();
        ESP::worldBulkheadDCs.clear();
        G::worldBulkheadMtx.unlock();
    }
}

void Hooks::hkApplication_Quit(int32_t exitCode, MethodInfo* method)
{
    il2cppi_log_write("Application::Quit called. Shutting down...");
    G::gameQuit = true;
    G::running = false;
    Enemy::StopRefreshThread();
    Hooks::RemoveHooks(false);

    // Trampoline is invalid after RemoveHooks, call original directly
    app::Application_Quit(exitCode, method);
}

void Hooks::hkApplication_Quit_1(MethodInfo* method)
{
    il2cppi_log_write("Application::Quit_1 called. Shutting down...");
    G::gameQuit = true;
    G::running = false;
    Enemy::StopRefreshThread();
    Hooks::RemoveHooks(false);

    app::Application_Quit_1(method);
}

// Debug hook to get list of possible items
void Hooks::hkItemDataBlock_OnPostSetup(app::ItemDataBlock* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::ItemDataBlock*, MethodInfo*)>(hooks["ItemDataBlock_OnPostSetup"]);
    fpOFunc(__this, method);

    auto betterString = il2cppi_to_string(__this->fields._publicName_k__BackingField);
    auto termName = il2cppi_to_string(__this->fields._terminalItemShortName_k__BackingField);
    std::string itemStr = betterString + "    |    " + termName;
    il2cppi_log_write(itemStr);
}

void Hooks::hkCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method)
{
    if (G::showMenu)
        value = app::CursorLockMode__Enum::Confined;

    static auto fpOFunc = reinterpret_cast<void (*)(app::CursorLockMode__Enum, MethodInfo*)>(hooks["Cursor_set_lockState"]);
    fpOFunc(value, method);
}

void Hooks::hkCursor_set_visible(bool value, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(bool, MethodInfo*)>(hooks["Cursor_set_visible"]);
    fpOFunc(G::showMenu, method);
}

void Hooks::hkLocalPlayerAgent_Update(app::LocalPlayerAgent* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::LocalPlayerAgent*, MethodInfo*)>(hooks["LocalPlayerAgent_Update"]);
    fpOFunc(__this, method);

    G::localPlayer = reinterpret_cast<app::PlayerAgent*>(__this);

    while (!G::callbacks.empty())
    {
        G::callbacks.front()();
        G::callbacks.pop();
    }

    if (G::mainCamera != NULL)
    {
        std::lock_guard<std::mutex> lock(G::matrixMtx);
        G::w2CamMatrix = app::Camera_get_worldToCameraMatrix(G::mainCamera, NULL);
        G::projMatrix = app::Camera_get_projectionMatrix(G::mainCamera, NULL);
        G::viewMatrix = Math::MatrixMult(G::projMatrix, G::w2CamMatrix);
    }

    // Handle custom fullbright light
    static app::Light* fullBrightLight = nullptr;

    if (Player::fullBrightToggleKey.isToggled())
    {
        // Create the light if it doesn't exist
        if (fullBrightLight == nullptr && G::mainCamera != nullptr)
        {
            // Get the camera's GameObject
            auto cameraGameObject = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(G::mainCamera), NULL);
            if (cameraGameObject)
            {
                // Get UnityEngine.CoreModule assembly
                auto domain = il2cpp_domain_get();
                auto assembly = il2cpp_domain_assembly_open(domain, "UnityEngine.CoreModule");
                if (assembly)
                {
                    auto image = il2cpp_assembly_get_image(assembly);
                    if (image)
                    {
                        // Get Light class
                        auto lightClass = il2cpp_class_from_name(image, "UnityEngine", "Light");
                        if (lightClass)
                        {
                            auto lightType = il2cpp_class_get_type(lightClass);
                            auto lightTypeObject = il2cpp_type_get_object(lightType);
                            // Add Light component to camera's GameObject
                            auto lightComponent = app::GameObject_AddComponent(cameraGameObject, reinterpret_cast<app::Type*>(lightTypeObject), NULL);
                            fullBrightLight = reinterpret_cast<app::Light*>(lightComponent);

                            if (fullBrightLight)
                            {
                                // Configure as spotlight
                                app::Light_set_type(fullBrightLight, app::LightType__Enum::Spot, NULL);

                                // Disable shadows for performance
                                app::Light_set_shadows(fullBrightLight, app::LightShadows__Enum::None, NULL);

                                // Set initial properties
                                app::Light_set_range(fullBrightLight, Player::fullBrightRange, NULL);
                                app::Light_set_intensity(fullBrightLight, Player::fullBrightIntensity, NULL);
                                app::Light_set_spotAngle(fullBrightLight, Player::fullBrightAngle, NULL);

                                app::Color lightColor;
                                lightColor.r = Player::fullBrightColor.x;
                                lightColor.g = Player::fullBrightColor.y;
                                lightColor.b = Player::fullBrightColor.z;
                                lightColor.a = Player::fullBrightColor.w;
                                app::Light_set_color(fullBrightLight, lightColor, NULL);
                            }
                        }
                    }
                }
            }
        }

        // Update light properties every frame from sliders
        // Use Unity's op_Implicit check to detect destroyed objects
        if (fullBrightLight != nullptr && !app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(fullBrightLight), NULL))
            fullBrightLight = nullptr;

        if (fullBrightLight != nullptr)
        {
            app::Light_set_range(fullBrightLight, Player::fullBrightRange, NULL);
            app::Light_set_intensity(fullBrightLight, Player::fullBrightIntensity, NULL);
            app::Light_set_spotAngle(fullBrightLight, Player::fullBrightAngle, NULL);

            app::Color lightColor;
            lightColor.r = Player::fullBrightColor.x;
            lightColor.g = Player::fullBrightColor.y;
            lightColor.b = Player::fullBrightColor.z;
            lightColor.a = Player::fullBrightColor.w;
            app::Light_set_color(fullBrightLight, lightColor, NULL);
        }
    }
    else if (fullBrightLight != nullptr)
    {
        // Destroy the light component when toggle is off
        if (app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(fullBrightLight), NULL))
            app::Object_1_Destroy_1(reinterpret_cast<app::Object_1*>(fullBrightLight), NULL);
        fullBrightLight = nullptr;
    }

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

    G::worldBulkheadMtx.lock();
    for (ESP::WorldBulkheadDC& i : ESP::worldBulkheadDCs) i.update();
    std::sort(ESP::worldBulkheadDCs.begin(), ESP::worldBulkheadDCs.end(), std::greater<ESP::WorldBulkheadDC>());
    G::worldBulkheadMtx.unlock();

    Aimbot::RunAimbot();
}

bool Hooks::hkDam_EnemyDamageBase_ProcessReceivedDamage(app::Dam_EnemyDamageBase* __this, float damage, app::Agent* damageSource,
    app::Vector3 position, app::Vector3 direction, app::ES_HitreactType__Enum hitreact, bool tryForceHitreact, int32_t limbID,
    float staggerDamageMulti, app::DamageNoiseLevel__Enum damageNoiseLevel,
    uint32_t gearCategoryId, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<bool (*)(app::Dam_EnemyDamageBase*, float, app::Agent*, app::Vector3, app::Vector3, app::ES_HitreactType__Enum, bool, int32_t, float, app::DamageNoiseLevel__Enum, uint32_t, MethodInfo*)>(hooks["Dam_EnemyDamageBase_ProcessReceivedDamage"]);

    if (Player::outgoingDamageToggleKey.isToggled() && damageSource == reinterpret_cast<app::Agent*>(G::localPlayer))
    {
        if (damage == 0.0f && s_pendingBulletDamage > 0.0f)
            damage = s_pendingBulletDamage;  // recover from UFloat16 underflow (e.g. Immortal's HealthMax=9999999)
        damage *= Player::outgoingDamageMulti;
    }
    s_pendingBulletDamage = 0.0f;

    bool retVal = fpOFunc(__this, damage, damageSource, position, direction, hitreact, tryForceHitreact, limbID, staggerDamageMulti, damageNoiseLevel, gearCategoryId, method);

    if ((*app::SNet__TypeInfo)->static_fields->_IsMaster_k__BackingField)
    {
        auto syncedDmg = reinterpret_cast<app::Dam_SyncedDamageBase*>(__this);
        //std::cout << "Trying to set health to: " << std::to_string(syncedDmg->fields._Health_k__BackingField) << std::endl;
        app::Dam_SyncedDamageBase_SendSetHealth(syncedDmg, syncedDmg->fields._Health_k__BackingField, NULL);
    }

    return retVal;
}

float Hooks::hkArchetypeDataBlock_GetSentryDamage(app::ArchetypeDataBlock* __this, app::PlayerAgent* owner, float distance, bool targetIsTagged, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<float (*)(app::ArchetypeDataBlock*, app::PlayerAgent*, float, bool, MethodInfo*)>(hooks["ArchetypeDataBlock_GetSentryDamage"]);
    float result = fpOFunc(__this, owner, distance, targetIsTagged, method);

    if (Player::turretDamageToggleKey.isToggled())
    {
        bool isLocal = (reinterpret_cast<app::Agent*>(owner) == reinterpret_cast<app::Agent*>(G::localPlayer));
        if (!Player::turretLocalOnly || isLocal)
            result *= Player::turretDamageMulti;
    }
    return result;
}

float Hooks::hkDam_EnemyDamageLimb_ApplyWeakspotAndArmorModifiers(app::Dam_EnemyDamageLimb* __this, float dam, float precisionMulti, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<float (*)(app::Dam_EnemyDamageLimb*, float, float, MethodInfo*)>(hooks["Dam_EnemyDamageLimb_ApplyWeakspotAndArmorModifiers"]);

    if (Player::forceWeakspotToggleKey.isToggled() && __this->fields.m_type != app::eLimbDamageType__Enum::Weakspot)
    {
        auto limbs = __this->fields.m_base->fields.DamageLimbs;
        if (limbs)
        {
            app::Dam_EnemyDamageLimb* bestLimb = nullptr;
            float bestMulti = 0.0f;
            for (il2cpp_array_size_t i = 0; i < limbs->max_length; i++)
            {
                auto limb = limbs->vector[i];
                if (limb && limb->fields.m_type == app::eLimbDamageType__Enum::Weakspot
                    && limb->fields.m_weakspotDamageMulti > bestMulti)
                {
                    bestMulti = limb->fields.m_weakspotDamageMulti;
                    bestLimb = limb;
                }
            }
            if (bestLimb)
            {
                float effective = bestMulti * precisionMulti;
                if (effective < 1.0f) effective = 1.0f;
                float result = effective * dam * bestLimb->fields.m_armorDamageMulti;
                s_pendingBulletDamage = result;
                return result;
            }
            // No weakspot limb found (e.g. Charger, Snatcher) — use 3x, the most common multiplier
            float effective = 3.0f * precisionMulti;
            if (effective < 1.0f) effective = 1.0f;
            float result = effective * dam;
            s_pendingBulletDamage = result;
            return result;
        }
    }

    float result = fpOFunc(__this, dam, precisionMulti, method);
    s_pendingBulletDamage = result;
    return result;
}

float Hooks::hkDam_EnemyDamageLimb_Custom_ApplyWeakspotAndArmorModifiers(app::Dam_EnemyDamageLimb_Custom* __this, float dam, float precisionMulti, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<float (*)(app::Dam_EnemyDamageLimb_Custom*, float, float, MethodInfo*)>(hooks["Dam_EnemyDamageLimb_Custom_ApplyWeakspotAndArmorModifiers"]);

    if (Player::forceWeakspotToggleKey.isToggled() && __this->fields._.m_type != app::eLimbDamageType__Enum::Weakspot)
    {
        auto limbs = __this->fields._.m_base->fields.DamageLimbs;
        if (limbs)
        {
            app::Dam_EnemyDamageLimb* bestLimb = nullptr;
            float bestMulti = 0.0f;
            for (il2cpp_array_size_t i = 0; i < limbs->max_length; i++)
            {
                auto limb = limbs->vector[i];
                if (limb && limb->fields.m_type == app::eLimbDamageType__Enum::Weakspot
                    && limb->fields.m_weakspotDamageMulti > bestMulti)
                {
                    bestMulti = limb->fields.m_weakspotDamageMulti;
                    bestLimb = limb;
                }
            }
            if (bestLimb)
            {
                float effective = bestMulti * precisionMulti;
                if (effective < 1.0f) effective = 1.0f;
                float result = effective * dam * bestLimb->fields.m_armorDamageMulti;
                s_pendingBulletDamage = result;
                return result;
            }
            // No weakspot limb found — use 3x, the most common multiplier
            float effective = 3.0f * precisionMulti;
            if (effective < 1.0f) effective = 1.0f;
            float result = effective * dam;
            s_pendingBulletDamage = result;
            return result;
        }
    }

    float result = fpOFunc(__this, dam, precisionMulti, method);
    s_pendingBulletDamage = result;
    return result;
}

void Hooks::hkRenderPipe_CameraUpdate(app::Camera* camera, app::RenderPipe_CameraData* cameraData, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::Camera*, app::RenderPipe_CameraData*, MethodInfo*)>(hooks["RenderPipe_CameraUpdate"]);

    // Call original
    fpOFunc(camera, cameraData, method);

    // Disable shadows when fullbright is on
    if (Player::fullBrightToggleKey.isToggled())
    {
        app::QualitySettings_set_shadowDistance(0.0f, NULL);
    }
}

void Hooks::hkPreLitVolume_Update(app::PreLitVolume* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::PreLitVolume*, MethodInfo*)>(hooks["PreLitVolume_Update"]);
    fpOFunc(__this, method);

    if (Player::noFogToggleKey.isToggled())
    {
        __this->fields.m_fogDensity = 0.0f;
        __this->fields.m_densityHeightAltitude = -9999.0f;
    }
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

void Hooks::hkLocalPlayerAgent_LateUpdate(app::LocalPlayerAgent* __this, MethodInfo* method)
{
    static auto fpOFunc = reinterpret_cast<void (*)(app::LocalPlayerAgent*, MethodInfo*)>(hooks["LocalPlayerAgent_LateUpdate"]);
    fpOFunc(__this, method);

    Enemy::RefreshEnemyAgents();
    Enemy::UpdateEnemyVisibility();
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
            // Load font with Chinese support
            ImFontConfig config;
            config.FontLoaderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;
            
            const ImWchar* chineseRanges = io.Fonts->GetGlyphRangesChineseFull();

            // Try multiple Chinese fonts in order of preference
            const char* chineseFonts[] = {
                "C:\\Windows\\Fonts\\msyh.ttc",    // Microsoft YaHei (Win7+)
                "C:\\Windows\\Fonts\\simhei.ttf",  // SimHei (older systems)
                "C:\\Windows\\Fonts\\simsun.ttc",  // SimSun (most compatible)
                "C:\\Windows\\Fonts\\msyhl.ttc",   // YaHei Light
                "C:\\Windows\\Fonts\\simkai.ttf",  // KaiTi
            };

            const char* loadedFontPath = nullptr;
            ImFont* menuFont = nullptr;
            for (const char* fontPath : chineseFonts) {
                // Check existence before calling AddFontFromFileTTF — it fires an assert on missing files
                if (FILE* f = fopen(fontPath, "rb")) {
                    fclose(f);
                    menuFont = io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &config, chineseRanges);
                    if (menuFont) {
                        loadedFontPath = fontPath;
                        break;
                    }
                }
            }

            if (loadedFontPath) {
                G::defaultFont = menuFont;
                G::espFont = io.Fonts->AddFontFromFileTTF(loadedFontPath, 14.0f, &config, chineseRanges);
                G::chineseFontAvailable = true;
            } else {
                G::defaultFont = io.Fonts->AddFontDefault();
                G::espFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
                    Fonts::GetRobotoFontDataTTFBase85(), 14, &config);
                G::chineseFontAvailable = false;
            }
            
            // Initialize language after font loading based on availability
            I18N::InitializeAfterFontLoad(G::chineseFontAvailable);

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