#pragma once
#include "InputUtil.h"
#include "imgui.h"


namespace Player
{
    extern KeyBindToggle godmodeToggleKey;
    extern KeyBindToggle infiAmmoToggleKey;
    extern KeyBindToggle glueInstantToggleKey;
    extern KeyBindToggle instaHackToggleKey;
    extern KeyBindToggle instantScanToggleKey;
    extern KeyBindToggle soloScanToggleKey;
    extern KeyBindToggle autoScanToggleKey;
    extern KeyBindToggle fullAutoToggleKey;
    extern KeyBindToggle noRecoilToggleKey;
    extern KeyBindToggle noSpreadToggleKey;
    extern KeyBindToggle noShakeToggleKey;
    extern KeyBindToggle noFogToggleKey;
    extern KeyBindToggle fullBrightToggleKey;

    extern KeyBindToggle speedModToggleKey;
    extern float speedMulti;

    extern KeyBindToggle outgoingDamageToggleKey;
    extern float outgoingDamageMulti;
    extern KeyBindToggle localDamageToggleKey;
    extern float localDamageMulti;
    extern KeyBindToggle teamDamageToggleKey;
    extern float teamDamageMulti;
    extern KeyBindToggle forceWeakspotToggleKey;
    extern KeyBindToggle turretDamageToggleKey;
    extern float turretDamageMulti;
    extern bool turretLocalOnly;

    extern KeyBind giveHealthKey;
    extern KeyBind giveDisinKey;
    extern KeyBind giveAmmoKey;

    extern KeyBindToggle autoSelfReviveToggleKey;
    extern KeyBind selfReviveKey;
    extern KeyBindToggle fullHpReviveToggleKey;

    // Full Bright Light Settings
    extern float fullBrightRange;
    extern float fullBrightIntensity;
    extern float fullBrightAngle;
    extern ImVec4 fullBrightColor;

    void UpdateInput();

    void GiveLocalHealth();
    void GiveLocalDisinfection();
    void GiveLocalAmmo();
    void SelfRevive();
}
