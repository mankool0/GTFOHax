#include "player.h"
#include "..\globals.h"

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

    KeyBind giveHealthKey;
    KeyBind giveDisinKey;
    KeyBind giveAmmoKey;

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
