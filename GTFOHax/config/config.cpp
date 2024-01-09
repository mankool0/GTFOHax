#include "config.h"
#include "..\globals.h"
#include "..\hacks\esp.h"
#include "..\hacks\player.h"
#include "..\hacks\aimbot.h"
#include "helpers.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define JSON_GET(k, v) \
    try \
    { \
        j.at(k).get_to(v); \
    } \
    catch (nlohmann::detail::out_of_range& e) \
    { \
        il2cppi_log_write(e.what()); \
    } \

void to_json(nlohmann::json& j, KeyBind value)
{
    j = nlohmann::json
    {
        {"KeyCode", value.getKeyCode()}
    };
}
void from_json(const nlohmann::json& j, KeyBind& value)
{
    KeyBind::KeyCode temp;
    JSON_GET("KeyCode", temp);
    value.setKeyCode(temp);
}

void to_json(nlohmann::json& j, KeyBindToggle value)
{
    j = nlohmann::json
    {
        {"KeyCode", value.getKeyCode()},
        {"ToggledOn", value.isToggled()}
    };
}

void from_json(const nlohmann::json& j, KeyBindToggle& value)
{
    KeyBind::KeyCode temp;
    JSON_GET("KeyCode", temp);
    JSON_GET("ToggledOn", value.toggledOn);
    value.setKeyCode(temp);
}

void to_json(nlohmann::json& j, ImVec4 value)
{
    j = nlohmann::json
    {
        {"X", value.x},
        {"Y", value.y},
        {"Z", value.z},
        {"W", value.w}
    };
}

void from_json(const nlohmann::json& j, ImVec4& value)
{
    JSON_GET("X", value.x);
    JSON_GET("Y", value.y);
    JSON_GET("Z", value.z);
    JSON_GET("W", value.w);
}

namespace ESP
{
    void to_json(nlohmann::json& j, ESP::WorldESPItem* value)
    {
        j = nlohmann::json
        {
            {"Enabled", value->enabled},
            {"RenderDistance", value->renderDistance},
            {"RenderColor", value->renderColor}
        };
    }

    void from_json(const nlohmann::json& j, ESP::WorldESPItem*& value)
    {
        value = new WorldESPItem();
        JSON_GET("Enabled", value->enabled);
        JSON_GET("RenderDistance", value->renderDistance);
        JSON_GET("RenderColor", value->renderColor);
    }

    void to_json(nlohmann::json& j, ESP::AgentESPSection value)
    {
        j = nlohmann::json
        {
            {"Type", value.type},
            {"Show", value.show},
            {"ShowBoxes", value.showBoxes},
            {"BoxesColor", value.boxesColor},
            {"BoxesOutlineColor", value.boxesOutlineColor},
            {"ShowHealthBar", value.showHealthBar},
            {"HealthBarThickness", value.healthBarThickness},
            {"HealthBarText", value.healthBarText},
            {"HealthBarTextFull", value.healthBarTextFull},
            {"HealthBarTextColor", value.healthBarTextColor},
            {"HealthBarTextOutlineColor", value.healthBarTextOutlineColor},
            {"ShowInfo", value.showInfo},
            {"ShowName", value.showName},
            {"ShowType", value.showType},
            {"ShowHealth", value.showHealth},
            {"ShowDistance", value.showDistance},
            {"TextColor", value.textColor},
            {"TextOutlineColor", value.textOutlineColor},
            {"RenderDistance", value.renderDistance},
            {"ShowSkeleton", value.showSkeleton},
            {"SkeletonColor", value.skeletonColor},
            {"SkeletonRenderDistance", value.skeletonRenderDistance},
            {"SkeletonThickness", value.skeletonThickness}
        };
    }

    void from_json(const nlohmann::json& j, ESP::AgentESPSection& value)
    {
        JSON_GET("Type", value.type);
        JSON_GET("Show", value.show);
        JSON_GET("ShowBoxes", value.showBoxes);
        JSON_GET("BoxesColor", value.boxesColor);
        JSON_GET("BoxesOutlineColor", value.boxesOutlineColor);
        JSON_GET("ShowHealthBar", value.showHealthBar);
        JSON_GET("HealthBarThickness", value.healthBarThickness);
        JSON_GET("HealthBarText", value.healthBarText);
        JSON_GET("HealthBarTextFull", value.healthBarTextFull);
        JSON_GET("HealthBarTextColor", value.healthBarTextColor);
        JSON_GET("HealthBarTextOutlineColor", value.healthBarTextOutlineColor);
        JSON_GET("ShowInfo", value.showInfo);
        JSON_GET("ShowName", value.showName);
        JSON_GET("ShowType", value.showType);
        JSON_GET("ShowHealth", value.showHealth);
        JSON_GET("ShowDistance", value.showDistance);
        JSON_GET("TextColor", value.textColor);
        JSON_GET("TextOutlineColor", value.textOutlineColor);
        JSON_GET("RenderDistance", value.renderDistance);
        JSON_GET("ShowSkeleton", value.showSkeleton);
        JSON_GET("SkeletonColor", value.skeletonColor);
        JSON_GET("SkeletonRenderDistance", value.skeletonRenderDistance);
        JSON_GET("SkeletonThickness", value.skeletonThickness);
    }

    void to_json(nlohmann::json& j, ESP::AgentESP value)
    {
        j = nlohmann::json
        {
            {"ToggleKey", value.toggleKey},
            {"VisibleSection", value.visibleSec},
            {"NonVisibleSection", value.nonVisibleSec}
        };
    }

    void from_json(const nlohmann::json& j, ESP::AgentESP& value)
    {
        JSON_GET("ToggleKey", value.toggleKey);
        JSON_GET("VisibleSection", value.visibleSec);
        JSON_GET("NonVisibleSection", value.nonVisibleSec);
    }
};

namespace Aimbot
{
    void to_json(nlohmann::json& j, Aimbot::Settings value)
    {
        j = nlohmann::json
        {
            {"ToggleKey", value.toggleKey},
            {"HoldOnly", value.holdOnly},
            {"HoldKey", value.holdKey},
            {"SilentAim", value.silentAim},
            {"MagicBullet", value.magicBullet},
            {"VisibleOnly", value.visibleOnly},
            {"AimAtArmor", value.aimAtArmor},
            {"MaxDistance", value.maxDistance},
            {"Smoothing", value.smoothing},
            {"AimFOV", value.aimFov},
            {"AimFOVColor", value.aimFovColor},
            {"RenderFOV", value.renderFOV},
            {"Priority", value.priority}
        };
    }

    void from_json(const nlohmann::json& j, Aimbot::Settings& value)
    {
        JSON_GET("ToggleKey", value.toggleKey);
        JSON_GET("HoldOnly", value.holdOnly);
        JSON_GET("HoldKey", value.holdKey);
        JSON_GET("SilentAim", value.silentAim);
        JSON_GET("MagicBullet", value.magicBullet);
        JSON_GET("VisibleOnly", value.visibleOnly);
        JSON_GET("AimAtArmor", value.aimAtArmor);
        JSON_GET("MaxDistance", value.maxDistance);
        JSON_GET("Smoothing", value.smoothing);
        JSON_GET("AimFOV", value.aimFov);
        JSON_GET("AimFOVColor", value.aimFovColor);
        JSON_GET("RenderFOV", value.renderFOV);
        JSON_GET("Priority", value.priority);
    }
};

void Config::Save()
{
    try
    {
        json j = json
        {
            {"MenuKey", G::menuKey},
            {"UnloadKey", G::unloadKey},
            {"Watermark", G::watermark},
            {"WatermarkCorner", G::watermarkCorner},


            {"GodmodeToggleKey", Player::godmodeToggleKey},
            {"InfiAmmoToggleKey", Player::infiAmmoToggleKey},
            {"GlueInstantToggleKey", Player::glueInstantToggleKey},
            {"InstaHackToggleKey", Player::instaHackToggleKey},
            {"FullAutoToggleKey", Player::fullAutoToggleKey},
            {"NoRecoilToggleKey", Player::noRecoilToggleKey},
            {"NoSpreadToggleKey", Player::noSpreadToggleKey},
            {"NoShakeToggleKey", Player::noShakeToggleKey},
            {"GiveHealthKey", Player::giveHealthKey},
            {"GiveDisinKey", Player::giveDisinKey},
            {"GiveAmmoKey", Player::giveAmmoKey},


            {"WorldESPToggleKey", ESP::worldESPToggleKey},
            {"WorldESP", ESP::espItemsMap},
            

            {"EnemyESP", ESP::enemyESP},

            {"EnemyAimbot", Aimbot::settings}
        };

        std::ofstream fSettings(G::configPath);
        fSettings << std::setw(4) << j << std::endl;
    }
    catch (...)
    {
        il2cppi_log_write("Error saving config");
    }
}

void Config::Load()
{
    if (!std::filesystem::exists(G::configPath))
        return;
    
    try
    {
        std::ifstream fSettings(G::configPath);
        json j = json::parse(fSettings);

        JSON_GET("MenuKey", G::menuKey);
        JSON_GET("UnloadKey", G::unloadKey);
        JSON_GET("Watermark", G::watermark);
        JSON_GET("WatermarkCorner", G::watermarkCorner);


        JSON_GET("GodmodeToggleKey", Player::godmodeToggleKey);
        JSON_GET("InfiAmmoToggleKey", Player::infiAmmoToggleKey);
        JSON_GET("GlueInstantToggleKey", Player::glueInstantToggleKey);
        JSON_GET("InstaHackToggleKey", Player::instaHackToggleKey);
        JSON_GET("FullAutoToggleKey", Player::fullAutoToggleKey);
        JSON_GET("NoRecoilToggleKey", Player::noRecoilToggleKey);
        JSON_GET("NoSpreadToggleKey", Player::noSpreadToggleKey);
        JSON_GET("NoShakeToggleKey", Player::noShakeToggleKey);
        JSON_GET("GiveHealthKey", Player::giveHealthKey);
        JSON_GET("GiveDisinKey", Player::giveDisinKey);
        JSON_GET("GiveAmmoKey", Player::giveAmmoKey);


        JSON_GET("WorldESPToggleKey", ESP::worldESPToggleKey);
        std::map<std::string, ESP::WorldESPItem*> tempESPItems;
        JSON_GET("WorldESP", tempESPItems);
        tempESPItems.insert(ESP::espItemsMap.begin(), ESP::espItemsMap.end());
        ESP::espItemsMap = tempESPItems;


        JSON_GET("EnemyESP", ESP::enemyESP);

        JSON_GET("EnemyAimbot", Aimbot::settings);
    }
    catch (...)
    {
        il2cppi_log_write("Failed loading config");
    }
}