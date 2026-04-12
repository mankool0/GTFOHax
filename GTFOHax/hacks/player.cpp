#include "player.h"
#include "globals.h"

#include <iostream>

namespace Player
{
    KeyBindToggle godmodeToggleKey;
    KeyBindToggle infiAmmoToggleKey;
    KeyBindToggle glueInstantToggleKey;
    KeyBindToggle instaHackToggleKey;
    KeyBindToggle fullAutoToggleKey;
    KeyBindToggle noRecoilToggleKey;
    KeyBindToggle noSpreadToggleKey;
    KeyBindToggle noShakeToggleKey;
    KeyBindToggle noFogToggleKey;
    KeyBindToggle fullBrightToggleKey;

    KeyBindToggle outgoingDamageToggleKey;
    float outgoingDamageMulti  = 2.0f;
    KeyBindToggle localDamageToggleKey;
    float localDamageMulti     = 0.5f;
    KeyBindToggle teamDamageToggleKey;
    float teamDamageMulti      = 0.5f;
    KeyBindToggle forceWeakspotToggleKey;
    KeyBindToggle turretDamageToggleKey;
    float turretDamageMulti    = 2.0f;
    bool turretLocalOnly       = true;

    KeyBind giveHealthKey;
    KeyBind giveDisinKey;
    KeyBind giveAmmoKey;

    KeyBindToggle autoSelfReviveToggleKey;
    KeyBind selfReviveKey;
    KeyBindToggle fullHpReviveToggleKey;

    // Full Bright Light Settings
    float fullBrightRange = 1000.0f;
    float fullBrightIntensity = 0.6f;
    float fullBrightAngle = 180.0f;
    ImVec4 fullBrightColor = {1.0f, 0.9f, 0.7f, 1.0f}; // RGBA: warm yellowish

    void UpdateInput()
    {
        godmodeToggleKey.handleToggle();
        infiAmmoToggleKey.handleToggle();
        glueInstantToggleKey.handleToggle();
        instaHackToggleKey.handleToggle();
        fullAutoToggleKey.handleToggle();
        noRecoilToggleKey.handleToggle();
        noSpreadToggleKey.handleToggle();
        noShakeToggleKey.handleToggle();
        noFogToggleKey.handleToggle();
        fullBrightToggleKey.handleToggle();
        forceWeakspotToggleKey.handleToggle();
        outgoingDamageToggleKey.handleToggle();
        localDamageToggleKey.handleToggle();
        teamDamageToggleKey.handleToggle();
        turretDamageToggleKey.handleToggle();

        autoSelfReviveToggleKey.handleToggle();
        fullHpReviveToggleKey.handleToggle();

        if (giveHealthKey.isPressed())
            GiveLocalHealth();
        if (giveDisinKey.isPressed())
            GiveLocalDisinfection();
        if (giveAmmoKey.isPressed())
            GiveLocalAmmo();
        if (selfReviveKey.isPressed())
            SelfRevive();
    }

    void _GiveLocalHealth()
    {
        auto localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        if (!localPlayer)
            return;

        app::PlayerAgent_GiveHealth(localPlayer, localPlayer, 4.0f, nullptr);
    }

    void _GiveLocalDisinfection()
    {
        auto localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        if (!localPlayer)
            return;

        app::PlayerAgent_GiveDisinfection(localPlayer, localPlayer, 4.0f, nullptr);
    }

    void _GiveLocalAmmo()
    {
        auto localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        auto playerBackpack = app::PlayerBackpackManager_get_LocalBackpack(nullptr);
        if (!localPlayer || !playerBackpack)
            return;

        app::PlayerAgent_GiveAmmoRel(localPlayer, localPlayer, 4.0f, 4.0f, 4.0f, nullptr);
        app::PlayerAmmoStorage_UpdateAmmoInPack(playerBackpack->fields.AmmoStorage, app::AmmoType__Enum::ResourcePackRel, 99.0f, nullptr);
        app::PlayerAmmoStorage_UpdateAmmoInPack(playerBackpack->fields.AmmoStorage, app::AmmoType__Enum::CurrentConsumable, 99.0f, nullptr);
    }

    void GiveLocalHealth()
    {
        G::callbacks.push([] { _GiveLocalHealth(); });
    }

    void GiveLocalDisinfection()
    {
        G::callbacks.push([] { _GiveLocalDisinfection(); });
    }

    void GiveLocalAmmo()
    {
        G::callbacks.push([] { _GiveLocalAmmo(); });
    }

    void _SelfRevive()
    {
        auto localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
        if (!localPlayer) return;
        app::Vector3 pos = localPlayer->fields._.m_position;
        app::AgentReplicatedActions_PlayerReviveAction(localPlayer, localPlayer, pos, nullptr);
    }

    void SelfRevive()
    {
        G::callbacks.push([] { _SelfRevive(); });
    }
}
