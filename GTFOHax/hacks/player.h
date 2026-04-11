#pragma once
#include "InputUtil.h"
#include "imgui.h"


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
    extern KeyBindToggle noFogToggleKey;
    extern KeyBindToggle fullBrightToggleKey;

    extern bool outgoingDamageEnabled;
    extern float outgoingDamageMulti;
    extern bool localDamageEnabled;
    extern float localDamageMulti;
    extern bool teamDamageEnabled;
    extern float teamDamageMulti;
    extern KeyBindToggle forceWeakspotToggleKey;
    extern bool turretDamageEnabled;
    extern float turretDamageMulti;
    extern bool turretLocalOnly;

    extern KeyBind giveHealthKey;
    extern KeyBind giveDisinKey;
    extern KeyBind giveAmmoKey;

    // Full Bright Light Settings
    extern float fullBrightRange;
    extern float fullBrightIntensity;
    extern float fullBrightAngle;
    extern ImVec4 fullBrightColor;

    void UpdateInput();

    void GiveLocalHealth();
    void GiveLocalDisinfection();
    void GiveLocalAmmo();
}
