#pragma once
#include "globals.h"

namespace Hooks
{
    void InitHooks();
    void RemoveHooks();
    HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    bool hkDam_PlayerDamageBase_OnIncomingDamage(app::Dam_PlayerDamageBase* __this, float damage, float originalDamage, app::Agent* source, MethodInfo* method);
    void hkDam_PlayerDamageBase_ModifyInfection(app::Dam_PlayerDamageBase* __this, app::pInfection data, bool sync, bool updatePageMap, MethodInfo* method);
    void hkDam_PlayerDamageLocal_Hitreact(app::Dam_PlayerDamageLocal* __this, float damage, app::Vector3 direction, bool triggerCameraShake, bool triggerGenericDialog, bool pushPlayer, MethodInfo* method);
    app::Vector2 hkFPS_RecoilSystem_ApplyRecoil(app::FPS_RecoilSystem* __this, bool resetSimilarity, app::RecoilDataBlock* recoilData, MethodInfo* method);
    bool hkWeapon_CastWeaponRay(app::Transform* muzzle, app::Weapon_WeaponHitData** weaponRayData, int32_t altRayCastMask, MethodInfo* method);
    bool hkWeapon_CastWeaponRay_1(app::Transform* alignTransform, app::Weapon_WeaponHitData** weaponRayData, app::Vector3 originPos, int32_t altRayCastMask, MethodInfo* method);
    float hkPlayerAmmoStorage_UpdateBulletsInPack(app::PlayerAmmoStorage* __this, app::AmmoType__Enum ammoType, int32_t bulletCount, MethodInfo* method);
    void hkBulletWeapon_Fire(app::BulletWeapon* __this, bool resetRecoilSimilarity, MethodInfo* method);
    void hkShotgun_Fire(app::Shotgun* __this, bool resetRecoilSimilarity, MethodInfo* method);
    void hkGlueGun_Updatepressure(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method);
    void hkGlueGun_UpdateRecharging(app::GlueGun* __this, app::PlayerInventoryBase_pSimpleItemSyncData* syncData, MethodInfo* method);
    void hkHackingMinigame_TimingGrid_StartGame(app::HackingMinigame_TimingGrid* __this, MethodInfo* method);
    void hkBulletWeaponArchetype_Update(app::BulletWeaponArchetype* __this, MethodInfo* method);
    void hkItemDataBlock_OnPostSetup(app::ItemDataBlock* __this, MethodInfo* method);
    
    void hkArtifactPickup_Core_Setup(app::ArtifactPickup_Core* __this, app::ArtifactCategory__Enum category, MethodInfo* method);
    void hkCommodityPickup_Core_Setup(app::CommodityPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkConsumablePickup_Core_Setup(app::ConsumablePickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkCarryItemPickup_Core_Setup(app::CarryItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkKeyItemPickup_Core_Setup(app::KeyItemPickup_Core* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkGenericSmallPickupItem_Core_Setup(app::GenericSmallPickupItem_Core* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkResourcePackPickup_Setup(app::ResourcePackPickup* __this, app::ItemDataBlock* data, MethodInfo* method);
    void hkLG_HSU_Setup(app::LG_HSU* __this, MethodInfo* method);
    void hkLG_BulkheadDoorController_Core_Setup(app::LG_BulkheadDoorController_Core* __this, MethodInfo* method);

    void hkLG_ComputerTerminal_Setup(app::LG_ComputerTerminal* __this, app::TerminalStartStateData* startStateData, app::TerminalPlacementData* terminalPlacementData, MethodInfo* method);

    void hkGameStateManager_ChangeState(app::eGameStateName__Enum nextState, MethodInfo* method);

    void hkCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method);
    void hkCursor_set_visible(bool value, MethodInfo* method);

    void hkLocalPlayerAgent_Update(app::LocalPlayerAgent* __this, MethodInfo* method);
    bool hkDam_EnemyDamageBase_ProcessReceivedDamage(app::Dam_EnemyDamageBase* __this, float damage, app::Agent* damageSource,
        app::Vector3 position, app::Vector3 direction, app::ES_HitreactType__Enum hitreact, bool tryForceHitreact, int32_t limbID,
        float staggerDamageMulti, app::DamageNoiseLevel__Enum damageNoiseLevel,
        uint32_t gearCategoryId, MethodInfo* method);

    void hkPreLitVolume_Update(app::PreLitVolume* __this, MethodInfo* method);
}

