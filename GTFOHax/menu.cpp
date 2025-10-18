#include "menu.h"
#include "globals.h"
#include "hacks\esp.h"
#include "hacks\player.h"
#include "hacks\enemy.h"
#include "hacks\aimbot.h"
#include "config\config.h"
#include "ui.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <helpers.h>
#include "math.h"

const char* tabs[] = {
    "Player",
    "ESP",
    "Aimbot",
    "Misc",
    "Config"
};

const char* corners[] = {
    "Top-Left",
    "Top-Right",
    "Bottom-Left",
    "Bottom-Right"
};

ImVec2 RenderESPText(ImVec2 drawPos, ImU32 color, ImU32 outlineColor, std::string text, bool centered = true, bool swapHeight = false)
{
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    float horizOffset = centered ? textSize.x / 2 : 0.0f;
    float vertOffset = swapHeight ? textSize.y : 0.0f;
    ImGui::PushFont(G::espFont);
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(drawPos.x - horizOffset + 1, drawPos.y - vertOffset + 1), outlineColor, text.c_str());
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(drawPos.x - horizOffset - 1, drawPos.y - vertOffset - 1), outlineColor, text.c_str());
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(drawPos.x - horizOffset + 1, drawPos.y - vertOffset - 1), outlineColor, text.c_str());
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(drawPos.x - horizOffset - 1, drawPos.y - vertOffset + 1), outlineColor, text.c_str());
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(drawPos.x - horizOffset, drawPos.y - vertOffset), color, text.c_str());
    ImGui::PopFont();

    return textSize;
}

void RenderWatermark()
{
    if (!G::watermark)
        return;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (G::watermarkCorner != -1)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (G::watermarkCorner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (G::watermarkCorner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (G::watermarkCorner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (G::watermarkCorner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Watermark", 0, window_flags);
    ImGui::PushFont(G::espFont);
    static const std::string version = LIBRARY_VERSION;
    static const std::string watermarkText = "UnKnoWnCheaTs.me | mankool | V" + version;
    ImGui::Text(watermarkText.c_str());
    ImGui::PopFont();
    ImGui::End();
}

void RenderTabPlayer()
{
    ImGui::Checkbox("Godmode", &Player::godmodeToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("GodmodeHotkey");
    ImGui::Hotkey("", Player::godmodeToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("Infinite Ammo", &Player::infiAmmoToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("InfiAmmoHotkey");
    ImGui::Hotkey("", Player::infiAmmoToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("Gluegun No Cooldown", &Player::glueInstantToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("GlueInstantHotKey");
    ImGui::Hotkey("", Player::glueInstantToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("Instant Hack", &Player::instaHackToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("InstaHackHotKey");
    ImGui::Hotkey("", Player::instaHackToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("Full Auto", &Player::fullAutoToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("FullAutoHotKey");
    ImGui::Hotkey("", Player::fullAutoToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("No Recoil", &Player::noRecoilToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("NoRecoilHotkey");
    ImGui::Hotkey("", Player::noRecoilToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("No Spread", &Player::noSpreadToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("NoSpreadHotkey");
    ImGui::Hotkey("", Player::noSpreadToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("No Shake", &Player::noShakeToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("NoShakeHotkey");
    ImGui::Hotkey("", Player::noShakeToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("No Fog", &Player::noFogToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("NoFogHotkey");
    ImGui::Hotkey("", Player::noFogToggleKey);
    ImGui::PopID();

    if (ImGui::Button("Give Health"))
        Player::GiveLocalHealth();
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("GiveHealthHotkey");
    ImGui::Hotkey("", Player::giveHealthKey);
    ImGui::PopID();

    if (ImGui::Button("Give Disinfection"))
        Player::GiveLocalDisinfection();
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("GiveDisinHotkey");
    ImGui::Hotkey("", Player::giveDisinKey);
    ImGui::PopID();

    if (ImGui::Button("Give Ammo"))
        Player::GiveLocalAmmo();
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("GiveAmmoHotkey");
    ImGui::Hotkey("", Player::giveAmmoKey);
    ImGui::PopID();
}

void RenderAgentESPSection(ESP::AgentESPSection& section)
{
    ImGui::Checkbox(("Enabled##EnemyCheckbox" + section.type).c_str(), &section.show);

    if (ImGui::TreeNode("Boxes"))
    {
        ImGui::Checkbox(("2D Boxes##Enemy" + section.type).c_str(), &section.showBoxes);
        ImGui::SameLine();
        ImGui::ColorEdit4(("##EnemyBoxesColor" + section.type).c_str(), (float*)&section.boxesColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        section.boxesOutlineColor.w = section.boxesColor.w;

        ImGui::Checkbox(("Healthbar##Enemy" + section.type).c_str(), &section.showHealthBar);
        ImGui::Text("Bar Thickness");
        ImGui::PushItemWidth(-21);
        ImGui::SliderInt(("##SliderEnemyHealthBarThickness" + section.type).c_str(), &section.healthBarThickness, 1, 20);

        ImGui::Checkbox(("Healthbar Text##Enemy" + section.type).c_str(), &section.healthBarText);
        ImGui::SameLine();
        ImGui::ColorEdit4(("##EnemyHealthBarTextColor" + section.type).c_str(), (float*)&section.healthBarTextColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        section.healthBarTextOutlineColor.w = section.healthBarTextColor.w;
        ImGui::Checkbox(("Healthbar Full Health Text##Enemy" + section.type).c_str(), &section.healthBarTextFull);
        
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Enemy Info"))
    {
        ImGui::Checkbox(("Enabled##EnemyInfo" + section.type).c_str(), &section.showInfo);
        ImGui::SameLine();
        ImGui::ColorEdit4(("##EnemyTextColor" + section.type).c_str(), (float*)&section.textColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        section.textOutlineColor.w = section.textColor.w;
                
        ImGui::Checkbox(("Name##EnemyName" + section.type).c_str(), &section.showName);
        ImGui::Checkbox(("Type##EnemyType" + section.type).c_str(), &section.showType);
        ImGui::Checkbox(("Health##EnemyHealth" + section.type).c_str(), &section.showHealth);
        ImGui::Checkbox(("Distance##EnemyDistance" + section.type).c_str(), &section.showDistance);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Skeleton"))
    {
        ImGui::Checkbox(("Enabled##EnemySkeleton" + section.type).c_str(), &section.showSkeleton);
        ImGui::SameLine();
        ImGui::ColorEdit4(("##EnemySkeletonColor" + section.type).c_str(), (float*)&section.skeletonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::Text("Render Distance");
        ImGui::PushItemWidth(-21);
        ImGui::SliderInt(("##SliderEnemySkeletonDistance" + section.type).c_str(), &section.skeletonRenderDistance, 0, 500);

        ImGui::Text("Line Thickness");
        ImGui::PushItemWidth(-21);
        ImGui::SliderFloat(("##SliderEnemySkeletonThickness" + section.type).c_str(), &section.skeletonThickness, 0.0f, 10.0f);

        ImGui::TreePop();
    }

    ImGui::Text("Render Distance");
    ImGui::PushItemWidth(-21);
    ImGui::SliderInt(("##SliderEnemyDistance" + section.type).c_str(), &section.renderDistance, 0, 500);
}

void RenderTabESP()
{
    ImGui::Checkbox("World ESP", &ESP::worldESPToggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("WorldESPHotkey");
    ImGui::Hotkey("", ESP::worldESPToggleKey);
    ImGui::PopID();

    ImGui::Checkbox("Enemy ESP", &ESP::enemyESP.toggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160);
    ImGui::PushID("EnemyESPHotkey");
    ImGui::Hotkey("", ESP::enemyESP.toggleKey);
    ImGui::PopID();

    if (ImGui::CollapsingHeader("World"))
    {
        ImGui::SetCursorPosX(205);
        ImGui::Text("Render Distance");

        
        for (auto it = ESP::espItemsReverse.begin(); it != ESP::espItemsReverse.end(); ++it)
        {
            std::string name = (*it).first;
            std::string code = (*it).second;
            ESP::WorldESPItem* itemSetting = ESP::espItemsMap[code];
            ImGui::Text(name.c_str());
            ImGui::SameLine();
            ImGui::SetCursorPosX(177);
            ImGui::Checkbox(("##Checkbox" + name).c_str(), &itemSetting->enabled);
            ImGui::SameLine();
            ImGui::PushItemWidth(-25);
            ImGui::SliderInt(("##Slider" + name).c_str(), &itemSetting->renderDistance, 0, 500);
            ImGui::SameLine();
            ImGui::ColorEdit4(("##Color" + name).c_str(), (float*)&itemSetting->renderColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            itemSetting->outlineColor.w = itemSetting->renderColor.w;
        }
    }
    
    if (ImGui::CollapsingHeader("Enemy"))
    {
        // TODO: Add options to show different enemy types:
        //Weakling = 0x00000000,
        //Standard = 0x00000001,
        //Special = 0x00000002,
        //MiniBoss = 0x00000003,
        //Boss = 0x00000004,

        if (ImGui::TreeNode("Visible"))
        {
            RenderAgentESPSection(ESP::enemyESP.visibleSec);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Non-Visible"))
        {
            RenderAgentESPSection(ESP::enemyESP.nonVisibleSec);
            ImGui::TreePop();
        }
    }
}

void RenderTabAimbot()
{
    ImGui::Checkbox("Enemy Aimbot", &Aimbot::settings.toggleKey.toggledOn);
    ImGui::SameLine();
    ImGui::PushID("EnemyAimbotHotkey");
    ImGui::Hotkey("", Aimbot::settings.toggleKey);
    ImGui::PopID();

    ImGui::Checkbox("On Hold", &Aimbot::settings.holdOnly);
    ImGui::SameLine();
    ImGui::PushID("EnemyAimbotHoldKey");
    ImGui::Hotkey("", Aimbot::settings.holdKey);
    ImGui::PopID();

    ImGui::Checkbox("Silent Aim##EnemyAimbot", &Aimbot::settings.silentAim);
    ImGui::Checkbox("Magic Bullet##EnemyAimbot", &Aimbot::settings.magicBullet);
    ImGui::Checkbox("Visible Only##EnemyAimbot", &Aimbot::settings.visibleOnly);
    ImGui::Checkbox("Aim At Armor##EnemyAimbot", &Aimbot::settings.aimAtArmor);
    
    ImGui::Text("Aim Distance");
    ImGui::SameLine();
    ImGui::PushItemWidth(-25);
    ImGui::SliderFloat("##AimDistanceSliderEnemyAimbot", &Aimbot::settings.maxDistance, 0.0f, 500.0f);
    
    ImGui::Text("Smoothing");
    ImGui::SameLine();
    ImGui::PushItemWidth(-25);
    ImGui::SliderFloat("##SmoothingSliderEnemyAimbot", &Aimbot::settings.smoothing, 0.0f, 0.999f);
    
    ImGui::Checkbox("Render FOV##EnemyAimbot", &Aimbot::settings.renderFOV);
    ImGui::SameLine();
    ImGui::PushItemWidth(-25);
    ImGui::SliderFloat("##SliderEnemyAimbotFOV", &Aimbot::settings.aimFov, 0.0f, 360.0f);
    ImGui::SameLine();
    ImGui::ColorEdit4("##EnemyAimbotFOVColor", (float*)&Aimbot::settings.aimFovColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::PopItemWidth();

    //int* aimType = reinterpret_cast<int*>(&Aimbot::settings.aimType);
    //ImGui::Text("Aim Type");
    //ImGui::SameLine();
    //ImGui::Combo("##AimTypeEnemyAimbot", aimType, Aimbot::AimTypeItems, IM_ARRAYSIZE(Aimbot::AimTypeItems));
    
    int* aimPriority = reinterpret_cast<int*>(&Aimbot::settings.priority);
    ImGui::Text("Aim Priority");
    ImGui::SameLine();
    ImGui::Combo("##AimPriorityEnemyAimbot", aimPriority, Aimbot::EnemyPriorityItems, IM_ARRAYSIZE(Aimbot::EnemyPriorityItems));
}

void RenderTabMisc()
{
    ImGui::Hotkey("Toggle Menu Key", G::menuKey);
    
    if (ImGui::Button("Unload Menu"))
        G::running = false;
    ImGui::SameLine();
    ImGui::Text("Key");
    ImGui::SameLine();
    ImGui::PushID("UnloadMenuKey");
    ImGui::Hotkey("", G::unloadKey);
    ImGui::PopID();

    ImGui::Checkbox("Watermark", &G::watermark);
    ImGui::Combo("Location##Watermark", &G::watermarkCorner, corners, IM_ARRAYSIZE(corners));

    static int selectedIndex = 0;
    if (ImGui::BeginCombo("##SpawnEnemyCombo", Enemy::enemyNames[selectedIndex].c_str()))
    {
        for (int i = 0; i < Enemy::enemyNames.size(); ++i)
        {
            const bool isSelected = (selectedIndex == i);
            if (ImGui::Selectable(Enemy::enemyNames[i].c_str(), isSelected))
            {
                selectedIndex = i;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Enemy"))
        Enemy::SpawnEnemy(Enemy::enemyIDs[Enemy::enemyNames[selectedIndex]], app::AgentMode__Enum::Hibernate); // TODO: Fix map issue
}

void RenderTabConfig()
{
    if (ImGui::Button("Save"))
        Config::Save();

    ImGui::SameLine();

    if (ImGui::Button("Load"))
        Config::Load();
}

void RenderPickupItem(app::LG_PickupItem_Sync* pickupItem, app::pPickupItemState itemState, float distance, std::string itemCode, std::string terminalKey)
{
    static std::vector<std::string> nonexistantItems;

    if (itemState.status != app::ePickupItemStatus__Enum::PlacedInLevel)
        return;

    if (!G::mainCamera || (itemState.placement.position.x == 0.0f && itemState.placement.position.y == 0.0f && itemState.placement.position.z == 0.0f))
        return;

    ImVec2 w2sPos;
    if (!Math::WorldToScreen(itemState.placement.position, w2sPos))
        return;

    std::string itemName = il2cppi_to_string(pickupItem->fields.item->fields._PublicName_k__BackingField);
    if (itemName == "NEONATE HSU")
        itemName = "NEONATE_HSU";
    else if (itemName == "GLP Hormone mk 2")
        itemName = "GLP Hormone";

    if (itemCode.empty())
        itemCode = itemName;

    if (itemCode == "BACKUP_DATA_CUBE")
        itemCode = "DATA_CUBE";

    auto it = ESP::espItemsMap.find(itemCode);
    if (it == ESP::espItemsMap.end())
    {
        if (std::find(nonexistantItems.begin(), nonexistantItems.end(), itemName) == nonexistantItems.end())
        {
            nonexistantItems.push_back(itemName);
            std::string error = "ERROR: " + itemName + " does not exist in espItemsMap. Please report this.";
            il2cppi_log_write(error);
        }
#ifdef _DEBUG
        std::string espStr = itemName + " [" + std::to_string(llround(distance)) + "m]";
        ImU32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImU32 outlineColor = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        RenderESPText(w2sPos, color, outlineColor, espStr);
#endif // _DEBUG
        return;
    }

    auto espItemSetting = it->second;

    if (!espItemSetting->enabled || distance > espItemSetting->renderDistance)
        return;

    if (itemCode == "Partial Decoder" || itemCode == "Personnel ID")
        itemName = itemCode;
    else if (!terminalKey.empty())
        itemName = terminalKey;
    std::string espStr = itemName + " [" + std::to_string(llround(distance)) + "m]";

    ImU32 color = ImGui::GetColorU32(espItemSetting->renderColor);
    ImU32 outlineColor = ImGui::GetColorU32(espItemSetting->outlineColor);
    RenderESPText(w2sPos, color, outlineColor, espStr);
}

void RenderWorldItems()
{
    G::worldItemsMtx.lock();
    for (std::vector<ESP::WorldPickupItem>::iterator it = ESP::worldItems.begin(); it != ESP::worldItems.end(); ++it)
    {
        RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, "", "");
    }
    G::worldItemsMtx.unlock();
}

void RenderArtifacts()
{
    G::worldArtifMtx.lock();
    for (std::vector<ESP::WorldArtifactItem>::iterator it = ESP::worldArtifacts.begin(); it != ESP::worldArtifacts.end(); ++it)
    {
        std::string itemCode;
        switch ((*it).artifactItem->fields.m_artifactCategory)
        {
            case app::ArtifactCategory__Enum::Common:
                itemCode = "artifact_muted";
                break;
            case app::ArtifactCategory__Enum::Uncommon:
                itemCode = "artifact_bold";
                break;
            case app::ArtifactCategory__Enum::Rare:
                itemCode = "artifact_aggressive";
                break;
            case app::ArtifactCategory__Enum::_COUNT:
                // Should never happen, but we don't want to continue if it's invalid
                return;
        }

        RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, itemCode, ESP::espItems[itemCode]);
    }
    G::worldArtifMtx.unlock();
}

void RenderCarryItems()
{
    G::worldCarryMtx.lock();
    for (std::vector<ESP::WorldCarryItem>::iterator it = ESP::worldCarryItems.begin(); it != ESP::worldCarryItems.end(); ++it)
    {
        RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, "", il2cppi_to_string((*it).carryItem->fields.m_itemKey));
    }
    G::worldCarryMtx.unlock();
}

void RenderKeys()
{
    G::worldKeyMtx.lock();
    for (std::vector<ESP::WorldKeyItem>::iterator it = ESP::worldKeys.begin(); it != ESP::worldKeys.end(); ++it)
    {
        std::string keyName = il2cppi_to_string((*it).keyItem->fields.m_keyItem->fields.m_keyName);
        std::string terminalName = keyName + "_" + std::to_string((*it).keyItem->fields.m_keyItem->fields.m_keyNum);
        if (keyName.find("KEY_") != std::string::npos)
            keyName = "KEYCARD";
        RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, keyName, terminalName);
    }
    G::worldKeyMtx.unlock();
}

void RenderGenerics()
{
    G::worldGenericMtx.lock();
    for (std::vector<ESP::WorldGenericItem>::iterator it = ESP::worldGenerics.begin(); it != ESP::worldGenerics.end(); ++it)
    {
        switch ((*it).genericItem->fields.m_type)
        {
            case app::eSmallGenericPickupType__Enum::Generic:
                //std::cout << "Trying to render Generic Small Pickup" << std::endl;
                RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, "", il2cppi_to_string((*it).genericItem->fields.m_itemKey));
                break;
            case app::eSmallGenericPickupType__Enum::Personnel_Id:
                //(*it)->fields.m_graphics->fields.m_serial->fields.
                //std::cout << "Trying to render Personnel ID Small Pickup" << std::endl;
                RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, "Personnel ID", il2cppi_to_string((*it).genericItem->fields.m_itemKey));
                break;
        }
    }
    G::worldGenericMtx.unlock();
}

void RenderResourcePacks()
{
    G::worldResourcePackMtx.lock();
    for (std::vector<ESP::WorldResourceItem>::iterator it = ESP::worldResourcePacks.begin(); it != ESP::worldResourcePacks.end(); ++it)
    {
        RenderPickupItem((*it).pickupItem, (*it).state, (*it).distance, "", il2cppi_to_string((*it).resourceItem->fields.m_itemKey));
    }
    G::worldResourcePackMtx.unlock();
}

void RenderTerminals()
{
    auto espTerminalSetting = ESP::espItemsMap["Terminal"];
    if (!espTerminalSetting->enabled)
        return;

    G::worldTerminalsMtx.lock();
    for (std::vector<ESP::WorldTerminalItem>::iterator it = ESP::worldTerminals.begin(); it != ESP::worldTerminals.end(); ++it)
    {
        if (!G::mainCamera || ((*it).terminalItem->fields.m_position.x == 0.0f && (*it).terminalItem->fields.m_position.y == 0.0f && (*it).terminalItem->fields.m_position.z == 0.0f))
            continue;

        if ((*it).distance > espTerminalSetting->renderDistance)
            continue;

        ImVec2 w2sPos;
        if (!Math::WorldToScreen((*it).terminalItem->fields.m_position, w2sPos))
            continue;

        std::string terminalName = il2cppi_to_string((*it).terminalItem->fields._ItemKey_k__BackingField);
        std::string terminalPass = (*it).terminalItem->fields._IsPasswordProtected_k__BackingField ? "\nPassword: " + il2cppi_to_string((*it).terminalItem->fields.m_password) : "";
        std::string espStr = terminalName + " [" + std::to_string(llround((*it).distance)) + "m]" + terminalPass;

        ImU32 color = ImGui::GetColorU32(espTerminalSetting->renderColor);
        ImU32 outlineColor = ImGui::GetColorU32(espTerminalSetting->outlineColor);
        RenderESPText(w2sPos, color, outlineColor, espStr);
    }
    G::worldTerminalsMtx.unlock();
}

void RenderHSUs()
{
    auto espHSUSetting = ESP::espItemsMap["HSU"];
    if (!espHSUSetting->enabled)
        return;

    G::worldHSUMtx.lock();
    for (std::vector<ESP::WorldHSUItem>::iterator it = ESP::worldHSUItems.begin(); it != ESP::worldHSUItems.end(); ++it)
    {
        if (!G::mainCamera || !(*it).hsuItem->fields.m_isWardenObjective || ((*it).position.x == 0.0f && (*it).position.y == 0.0f && (*it).position.z == 0.0f))
            continue;

        if ((*it).distance > espHSUSetting->renderDistance)
            continue;

        ImVec2 w2sPos;
        if (!Math::WorldToScreen((*it).position, w2sPos))
            continue;

        std::string espStr = il2cppi_to_string((*it).hsuItem->fields.m_itemKey) + " [" + std::to_string(llround((*it).distance)) + "m]";

        ImU32 color = ImGui::GetColorU32(espHSUSetting->renderColor);
        ImU32 outlineColor = ImGui::GetColorU32(espHSUSetting->outlineColor);
        RenderESPText(w2sPos, color, outlineColor, espStr);
    }
    G::worldHSUMtx.unlock();
}

void RenderBulkheads()
{
    auto espBulkheadSetting = ESP::espItemsMap["Bulkhead"];
    if (!espBulkheadSetting->enabled)
        return;

    G::worldBulkheadMtx.lock();
    for (std::vector<ESP::WorldBulkheadDC>::iterator it = ESP::worldBulkheadDCs.begin(); it != ESP::worldBulkheadDCs.end(); ++it)
    {
        if (!G::mainCamera || ((*it).position.x == 0.0f && (*it).position.y == 0.0f && (*it).position.z == 0.0f))
            continue;

        if ((*it).distance > espBulkheadSetting->renderDistance)
            continue;

        ImVec2 w2sPos;
        if (!Math::WorldToScreen((*it).position, w2sPos))
            continue;

        std::string espStr = il2cppi_to_string((*it).bulkheadDC->fields.m_itemKey) + " [" + std::to_string(llround((*it).distance)) + "m]";

        ImU32 color = ImGui::GetColorU32(espBulkheadSetting->renderColor);
        ImU32 outlineColor = ImGui::GetColorU32(espBulkheadSetting->outlineColor);
        RenderESPText(w2sPos, color, outlineColor, espStr);
    }
    G::worldBulkheadMtx.unlock();
}

void RenderWorldESP()
{
    RenderWorldItems();
    RenderResourcePacks();
    RenderArtifacts();
    RenderTerminals();
    RenderKeys();
    RenderGenerics();
    RenderCarryItems();
    RenderHSUs();
    RenderBulkheads();
}

void DrawBone(ImU32 color, app::Vector3 startBone, app::Vector3 endBone, float thickness)
{
    if (!G::mainCamera)
        return;
    ImVec2 drawPosStart, drawPosEnd;
    if (!Math::WorldToScreen(startBone, drawPosStart) || !Math::WorldToScreen(endBone, drawPosEnd))
        return;
    ImGui::GetBackgroundDrawList()->AddLine(drawPosStart, drawPosEnd, color, thickness);
}

void DrawSkeleton(Enemy::EnemyInfo* fullInfo)
{
    for (std::vector<std::vector<app::HumanBodyBones__Enum>>::iterator itOuter = Enemy::skeletonBones.begin(); itOuter != Enemy::skeletonBones.end(); ++itOuter)
    { 
        Enemy::Bone prevBone;
        for (std::vector<app::HumanBodyBones__Enum>::iterator it = (*itOuter).begin(); it != (*itOuter).end(); ++it)
        {
            Enemy::Bone curBone = fullInfo->skeletonBones[*it]; // TODO: Fix map issue
            if (prevBone.position.x == 0.0f && prevBone.position.y == 0.0f && prevBone.position.z == 0.0f)
            {
                prevBone = curBone;
                continue;
            }
            if (curBone.position.x == 0.0f && curBone.position.y == 0.0f && curBone.position.z == 0.0f)
                continue;
            
            if (prevBone.visible || curBone.visible)
            {
                if (!ESP::enemyESP.visibleSec.showSkeleton || fullInfo->distance > ESP::enemyESP.visibleSec.skeletonRenderDistance)
                    continue;
                DrawBone(ImGui::GetColorU32(ESP::enemyESP.visibleSec.skeletonColor), prevBone.position, curBone.position, ESP::enemyESP.visibleSec.skeletonThickness);
            }
            else
            {
                if (!ESP::enemyESP.nonVisibleSec.showSkeleton || fullInfo->distance > ESP::enemyESP.nonVisibleSec.skeletonRenderDistance)
                    continue;
                DrawBone(ImGui::GetColorU32(ESP::enemyESP.nonVisibleSec.skeletonColor), prevBone.position, curBone.position, ESP::enemyESP.nonVisibleSec.skeletonThickness);
            }

            prevBone = curBone;
        }
    }
}

void RenderEnemyAgent(Enemy::EnemyInfo* enemyInfo, ESP::AgentESPSection* espSettings)
{
    if (enemyInfo->distance > espSettings->renderDistance)
        return;

    Enemy::Bone headBone = enemyInfo->useFallback ? enemyInfo->fallbackBone : enemyInfo->skeletonBones[app::HumanBodyBones__Enum::Head]; // TODO: Fix map issue
    ImVec2 w2sHead;
    if (!Math::WorldToScreen(headBone.position, w2sHead))
        return;

    ImVec2 w2sLeftFoot, w2sRightFoot;
    if (!enemyInfo->useFallback)
    {
        Enemy::Bone leftFootBone = enemyInfo->skeletonBones[app::HumanBodyBones__Enum::LeftFoot]; // TODO: Fix map issue
        Enemy::Bone rightFootBone = enemyInfo->skeletonBones[app::HumanBodyBones__Enum::RightFoot]; // TODO: Fix map issue
        if (!Math::WorldToScreen(leftFootBone.position, w2sLeftFoot) ||
            !Math::WorldToScreen(rightFootBone.position, w2sRightFoot))
            return;
    }
    else
    {
        w2sLeftFoot = w2sRightFoot = w2sHead;
    }
    
    ImVec2 min, max;
    if (!enemyInfo->useFallback)
    {
        min.y = min.x = (std::numeric_limits<float>::max)();
        max.y = max.x = -(std::numeric_limits<float>::max)();
    }
    else
    {
        min.x = max.x = w2sHead.x;
        min.y = max.y = w2sHead.y;
    }
    bool valid = true;
    for (auto const& [key, val] : enemyInfo->skeletonBones)
    {
        ImVec2 curPos;
        auto bonePos = val.position;
        if (!Math::WorldToScreen((bonePos), curPos))
        {
            valid = false;
            continue;
        }
        min.x = (std::min)(min.x, curPos.x);
        min.y = (std::min)(min.y, curPos.y);
        max.x = (std::max)(max.x, curPos.x);
        max.y = (std::max)(max.y, curPos.y);
    }

    if (espSettings->showBoxes && !enemyInfo->useFallback && valid)
    {    
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2{ min.x - 1.0f, min.y - 1.0f }, ImVec2{ max.x + 1.0f, max.y + 1.0f }, ImGui::GetColorU32(espSettings->boxesOutlineColor));
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2{ min.x + 1.0f, min.y + 1.0f }, ImVec2{ max.x - 1.0f, max.y - 1.0f }, ImGui::GetColorU32(espSettings->boxesOutlineColor));
        ImGui::GetBackgroundDrawList()->AddRect(min, max, ImGui::GetColorU32(espSettings->boxesColor));
    }


    auto enemyDamage = reinterpret_cast<app::Dam_SyncedDamageBase*>(enemyInfo->enemyAgent->fields.Damage);
    std::string enemyHealth = std::to_string(llround(enemyDamage->fields._Health_k__BackingField));
    if (espSettings->showHealthBar && !enemyInfo->useFallback && valid)
    {
        float health = enemyDamage->fields._Health_k__BackingField;
        float healthMax = enemyDamage->fields._HealthMax_k__BackingField;
        float healthRatio = health / healthMax;

        float maxHeight = max.y - min.y;
        float barWidth = espSettings->healthBarThickness + 4.0f;
        ImVec2 barTopRight = { min.x - 2, min.y };
        ImVec2 barBotLeft = { min.x - barWidth, min.y + maxHeight };


        ImVec2 healthBotLeft = { barBotLeft.x + 1, barBotLeft.y };
        float healthHeight = (maxHeight - 2) * healthRatio;
        ImVec2 healthTopRight = { barTopRight.x - 1, healthBotLeft.y - healthHeight - 2.0f};
        ImVec4 healthColor = { (std::min)((2.0f * (100 - (healthRatio * 100.0f))) / 100.0f, 1.0f),
            (std::min)((2.0f * (healthRatio * 100.0f)) / 100.0f, 1.0f),
            0.0f, 1.0f };

        ImGui::GetBackgroundDrawList()->AddRect(barBotLeft, barTopRight, IM_COL32_BLACK); //Draw healthbar outline
        ImGui::GetBackgroundDrawList()->AddRectFilled(healthBotLeft, healthTopRight, ImGui::GetColorU32(healthColor)); //Draw healthbar

        if (espSettings->healthBarText && !(healthRatio == 1.0f && !espSettings->healthBarTextFull))
            RenderESPText(ImVec2(healthTopRight.x - (espSettings->healthBarThickness / 2), healthTopRight.y), ImGui::GetColorU32(espSettings->healthBarTextColor), ImGui::GetColorU32(espSettings->healthBarTextOutlineColor), enemyHealth, true, true);
    }

    if (espSettings->showInfo)
    {
        
        std::string enemyName = "";
        if (espSettings->showName)
        {
            enemyName = enemyInfo->enemyObjectName;
            auto gPrefabIndex = enemyName.find("GeneratedPrefab");
            if (gPrefabIndex != std::string::npos)
            {
                enemyName = enemyName.substr(0, gPrefabIndex);
                std::replace(enemyName.begin(), enemyName.end(), '_', ' ');
                enemyName.erase(enemyName.find_last_not_of(' ') + 1);
            }
        }

        std::string enemyType = "";
        if (espSettings->showType)
        {
            switch (enemyInfo->enemyAgent->fields.EnemyData->fields._EnemyType_k__BackingField)
            {
            case app::eEnemyType__Enum::Weakling:
                enemyType = "Weakling";
                break;
            case app::eEnemyType__Enum::Standard:
                enemyType = "Standard";
                break;
            case app::eEnemyType__Enum::Special:
                enemyType = "Special";
                break;
            case app::eEnemyType__Enum::MiniBoss:
                enemyType = "MiniBoss";
                break;
            case app::eEnemyType__Enum::Boss:
                enemyType = "Boss";
                break;
            }
        }

        ImU32 color = ImGui::GetColorU32(espSettings->textColor);
        ImU32 outlineColor = ImGui::GetColorU32(espSettings->textOutlineColor);

        float center = (min.x + max.x) / 2.0f;
        ImVec2 topCenter = { center, min.y };
        ImVec2 bottomCenter = { center, max.y };
        ImVec2 typeTextSize = ImVec2();
        if (espSettings->showType)
            typeTextSize = RenderESPText(topCenter, color, outlineColor, enemyType, true, true);
        if (espSettings->showName)
            RenderESPText(ImVec2(topCenter.x, topCenter.y - typeTextSize.y), color, outlineColor, enemyName, true, true);


        ImVec2 distanceTextSize = ImVec2();
        if (espSettings->showDistance)
            distanceTextSize = RenderESPText(bottomCenter, color, outlineColor, "[" + std::to_string(llround(enemyInfo->distance)) + "m]", true, false);
        if (espSettings->showHealth)
            RenderESPText(ImVec2(bottomCenter.x, bottomCenter.y + distanceTextSize.y), color, outlineColor, "HP: " + enemyHealth, true, false);

    }

    DrawSkeleton(enemyInfo);
}

void RenderEnemyESP()
{
    G::enemyVecMtx.lock();
    std::vector<std::shared_ptr<Enemy::EnemyInfo>> enemies = Enemy::enemies;
    G::enemyVecMtx.unlock();
    std::sort(enemies.begin(), enemies.end(), [](const std::shared_ptr<Enemy::EnemyInfo> lhs, const std::shared_ptr<Enemy::EnemyInfo> rhs) {
        return lhs->distance > rhs->distance;
        });
    for (auto it = enemies.begin(); it != enemies.end(); ++it)
    {
        Enemy::EnemyInfo* enemyInfo = (*it).get();

        Enemy::Bone defBone;
        if (enemyInfo->useFallback)
            defBone = enemyInfo->fallbackBone;
        else
            defBone = enemyInfo->skeletonBones[app::HumanBodyBones__Enum::Head]; // TODO: Fix map issue

        if (!G::mainCamera || !(enemyInfo->enemyAgent->fields.m_alive) || (defBone.position.x == 0.0f && defBone.position.y == 0.0f && defBone.position.z == 0.0f))
            continue;

        if (enemyInfo->visible && ESP::enemyESP.visibleSec.show)
            RenderEnemyAgent(enemyInfo, &ESP::enemyESP.visibleSec);
        else if (!enemyInfo->visible && ESP::enemyESP.nonVisibleSec.show)
            RenderEnemyAgent(enemyInfo, &ESP::enemyESP.nonVisibleSec);
    }
}

void RenderAimbotESP()
{
    if (Aimbot::settings.renderFOV)
    {
        float vertFOV = app::Camera_get_fieldOfView(G::mainCamera, NULL);
        float vFOVRad = (float)ANG_RAD(vertFOV);
        float camHeight = tanf(vFOVRad * 0.5f);
        float camAspect = app::Camera_get_aspect(G::mainCamera, NULL);
        float hFOVRad = atanf(camHeight * camAspect) * 2.0f;

        float radAimbotFOV = (float)ANG_RAD(Aimbot::settings.aimFov);
        float circleRadius = (tanf(radAimbotFOV / 2.0f) / tanf(hFOVRad / 2.0f)) * (G::screenWidth / 2.0f);
        ImGui::GetBackgroundDrawList()->AddCircle({ G::screenWidth / 2.0f, G::screenHeight / 2.0f }, circleRadius, ImGui::GetColorU32(Aimbot::settings.aimFovColor));
    }
}

void RenderESP()
{
    if (app::GameStateManager_get_CurrentStateName(NULL) != app::eGameStateName__Enum::InLevel)
        return;

    G::localPlayer = app::PlayerManager_2_GetLocalPlayerAgent(nullptr);
    if (!G::localPlayer)
        return;

    if (G::mainCamera == NULL)
    {
        G::mainCamera = app::Camera_get_main(NULL);
        return;
    }

    G::screenHeight = app::Screen_get_height(NULL);
    G::screenWidth = app::Screen_get_width(NULL);

    if (ESP::enemyESP.toggleKey.isToggled())
        RenderEnemyESP();
    if (ESP::worldESPToggleKey.isToggled())
        RenderWorldESP();
    if (Aimbot::settings.toggleKey.isToggled())
        RenderAimbotESP();
}

void DrawMenu()
{
    static int page = 0;


    ImGui::Begin("GTFO Hax");
    for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
    {
        if (i == page)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));
            ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0));
            ImGui::PopStyleColor(3);
        }
        else if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
            page = i;

        if (i < IM_ARRAYSIZE(tabs) - 1)
            ImGui::SameLine();
    }

    ImGui::Separator();

    switch (page)
    {
    case 0:
        RenderTabPlayer();
        break;
    case 1:
        RenderTabESP();
        break;
    case 2:
        RenderTabAimbot();
        break;
    case 3:
        RenderTabMisc();
        break;
    case 4:
        RenderTabConfig();
        break;
    }

    ImGui::End();
}
