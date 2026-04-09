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

    KeyBind giveHealthKey;
    KeyBind giveDisinKey;
    KeyBind giveAmmoKey;

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

        if (giveHealthKey.isPressed())
            GiveLocalHealth();
        if (giveDisinKey.isPressed())
            GiveLocalDisinfection();
        if (giveAmmoKey.isPressed())
            GiveLocalAmmo();
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
}
