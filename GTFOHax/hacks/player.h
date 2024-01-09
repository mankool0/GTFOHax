#pragma once
#include "..\InputUtil.h"


namespace Player
{
    extern KeyBindToggle godmodeToggleKey;
    extern KeyBindToggle infiAmmoToggleKey;
    extern KeyBindToggle glueInstantToggleKey;
    extern KeyBindToggle instaHackToggleKey;
    extern KeyBindToggle fullAutoToggleKey;
    extern KeyBindToggle noRecoilToggleKey;
    extern KeyBindToggle noSpreadToggleKey;
    extern KeyBindToggle noShakeToggleKey;

    extern KeyBind giveHealthKey;
    extern KeyBind giveDisinKey;
    extern KeyBind giveAmmoKey;

    void UpdateInput();

    void GiveLocalHealth();
    void GiveLocalDisinfection();
    void GiveLocalAmmo();
}
