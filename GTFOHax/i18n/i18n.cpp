#include "i18n.h"
#include <Windows.h>

namespace I18N {
    static Language currentLanguage = Language::English;

    const char* LanguageNames[] = {
        "English",
        "中文" // Chinese
    };

    const int LanguageCount = sizeof(LanguageNames) / sizeof(LanguageNames[0]);

    static std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations = {
        {"tab_player", {{Language::English, "Player"}, {Language::Chinese, "玩家"}}},
        {"tab_esp", {{Language::English, "ESP"}, {Language::Chinese, "透视"}}},
        {"tab_aimbot", {{Language::English, "Aimbot"}, {Language::Chinese, "自瞄"}}},
        {"tab_misc", {{Language::English, "Misc"}, {Language::Chinese, "杂项"}}},
        {"tab_config", {{Language::English, "Config"}, {Language::Chinese, "配置"}}},
        {"player_godmode", {{Language::English, "Godmode"}, {Language::Chinese, "无敌模式"}}},
        {"player_infinite_ammo", {{Language::English, "Infinite Ammo"}, {Language::Chinese, "无限弹药"}}},
        {"player_gluegun_no_cooldown", {{Language::English, "Gluegun No Cooldown"}, {Language::Chinese, "胶枪无冷却"}}},
        {"player_instant_hack", {{Language::English, "Instant Hack"}, {Language::Chinese, "瞬间破解"}}},
        {"player_full_auto", {{Language::English, "Full Auto"}, {Language::Chinese, "全自动射击"}}},
        {"player_no_recoil", {{Language::English, "No Recoil"}, {Language::Chinese, "无后坐力"}}},
        {"player_no_spread", {{Language::English, "No Spread"}, {Language::Chinese, "无扩散"}}},
        {"player_no_shake", {{Language::English, "No Shake"}, {Language::Chinese, "无抖动"}}},
        {"player_no_fog", {{Language::English, "No Fog"}, {Language::Chinese, "无雾"}}},
        {"player_give_health", {{Language::English, "Give Health"}, {Language::Chinese, "给予生命"}}},
        {"player_give_disinfection", {{Language::English, "Give Disinfection"}, {Language::Chinese, "给予消毒"}}},
        {"player_give_ammo", {{Language::English, "Give Ammo"}, {Language::Chinese, "给予弹药"}}},
        {"esp_world_esp", {{Language::English, "World ESP"}, {Language::Chinese, "世界透视"}}},
        {"esp_enemy_esp", {{Language::English, "Enemy ESP"}, {Language::Chinese, "敌人透视"}}},
        {"esp_world", {{Language::English, "World"}, {Language::Chinese, "世界"}}},
        {"esp_enemy", {{Language::English, "Enemy"}, {Language::Chinese, "敌人"}}},
        {"esp_enabled", {{Language::English, "Enabled"}, {Language::Chinese, "启用"}}},
        {"esp_boxes", {{Language::English, "Boxes"}, {Language::Chinese, "方框"}}},
        {"esp_2d_boxes", {{Language::English, "2D Boxes"}, {Language::Chinese, "2D方框"}}},
        {"esp_healthbar", {{Language::English, "Healthbar"}, {Language::Chinese, "血条"}}},
        {"esp_healthbar_text", {{Language::English, "Healthbar Text"}, {Language::Chinese, "血条文字"}}},
        {"esp_healthbar_full_text", {{Language::English, "Healthbar Full Health Text"}, {Language::Chinese, "满血显示文字"}}},
        {"esp_bar_thickness", {{Language::English, "Bar Thickness"}, {Language::Chinese, "血条厚度"}}},
        {"esp_enemy_info", {{Language::English, "Enemy Info"}, {Language::Chinese, "敌人信息"}}},
        {"esp_name", {{Language::English, "Name"}, {Language::Chinese, "名称"}}},
        {"esp_type", {{Language::English, "Type"}, {Language::Chinese, "类型"}}},
        {"esp_health", {{Language::English, "Health"}, {Language::Chinese, "生命值"}}},
        {"esp_distance", {{Language::English, "Distance"}, {Language::Chinese, "距离"}}},
        {"esp_skeleton", {{Language::English, "Skeleton"}, {Language::Chinese, "骨骼"}}},
        {"esp_render_distance", {{Language::English, "Render Distance"}, {Language::Chinese, "渲染距离"}}},
        {"esp_line_thickness", {{Language::English, "Line Thickness"}, {Language::Chinese, "线条粗细"}}},
        {"esp_visible", {{Language::English, "Visible"}, {Language::Chinese, "可见"}}},
        {"esp_non_visible", {{Language::English, "Non-Visible"}, {Language::Chinese, "不可见"}}},
        {"esp_locked", {{Language::English, "LOCKED"}, {Language::Chinese, "锁定"}}},
        {"aimbot_enemy_aimbot", {{Language::English, "Enemy Aimbot"}, {Language::Chinese, "敌人自瞄"}}},
        {"aimbot_on_hold", {{Language::English, "On Hold"}, {Language::Chinese, "按住生效"}}},
        {"aimbot_silent_aim", {{Language::English, "Silent Aim"}, {Language::Chinese, "静默自瞄"}}},
        {"aimbot_magic_bullet", {{Language::English, "Magic Bullet"}, {Language::Chinese, "魔法子弹"}}},
        {"aimbot_magic_bullet_direction", {{Language::English, "Direction"}, {Language::Chinese, "射击方向"}}},
        {"aimbot_magic_bullet_offset", {{Language::English, "Offset"}, {Language::Chinese, "偏移距离"}}},
        {"aimbot_hit_ghost", {{Language::English, "Hit Ghost"}, {Language::Chinese, "命中残影"}}},
        {"aimbot_hit_ghost_duration", {{Language::English, "Duration"}, {Language::Chinese, "持续时间"}}},
        {"aimbot_hit_ghost_thickness", {{Language::English, "Thickness"}, {Language::Chinese, "线条粗细"}}},
        {"aimbot_bullet_ray", {{Language::English, "Bullet Ray"}, {Language::Chinese, "子弹射线"}}},
        {"aimbot_bullet_ray_duration", {{Language::English, "Duration"}, {Language::Chinese, "持续时间"}}},
        {"aimbot_bullet_ray_thickness", {{Language::English, "Thickness"}, {Language::Chinese, "线条粗细"}}},
        {"aimbot_target_highlight", {{Language::English, "Highlight Target"}, {Language::Chinese, "高亮锁定目标"}}},
        {"aimbot_target_marker", {{Language::English, "Show Marker"}, {Language::Chinese, "显示标记"}}},
        {"aimbot_visible_only", {{Language::English, "Visible Only"}, {Language::Chinese, "仅可见目标"}}},
        {"aimbot_aim_at_armor", {{Language::English, "Aim At Armor"}, {Language::Chinese, "瞄准护甲"}}},
        {"aimbot_aim_distance", {{Language::English, "Aim Distance"}, {Language::Chinese, "自瞄距离"}}},
        {"aimbot_smoothing", {{Language::English, "Smoothing"}, {Language::Chinese, "平滑度"}}},
        {"aimbot_render_fov", {{Language::English, "Render FOV"}, {Language::Chinese, "显示视野"}}},
        {"aimbot_aim_priority", {{Language::English, "Aim Priority"}, {Language::Chinese, "瞄准优先级"}}},
        {"misc_toggle_menu_key", {{Language::English, "Toggle Menu Key"}, {Language::Chinese, "菜单开关键"}}},
        {"misc_unload_menu", {{Language::English, "Unload Menu"}, {Language::Chinese, "卸载菜单"}}},
        {"misc_key", {{Language::English, "Key"}, {Language::Chinese, "按键"}}},
        {"misc_watermark", {{Language::English, "Watermark"}, {Language::Chinese, "水印"}}},
        {"misc_location", {{Language::English, "Location"}, {Language::Chinese, "位置"}}},
        {"misc_spawn_enemy", {{Language::English, "Spawn Enemy"}, {Language::Chinese, "生成敌人"}}},
        {"misc_language", {{Language::English, "Language"}, {Language::Chinese, "语言"}}},
        {"config_save", {{Language::English, "Save"}, {Language::Chinese, "保存"}}},
        {"config_load", {{Language::English, "Load"}, {Language::Chinese, "加载"}}},
        {"corner_top_left", {{Language::English, "Top-Left"}, {Language::Chinese, "左上"}}},
        {"corner_top_right", {{Language::English, "Top-Right"}, {Language::Chinese, "右上"}}},
        {"corner_bottom_left", {{Language::English, "Bottom-Left"}, {Language::Chinese, "左下"}}},
        {"corner_bottom_right", {{Language::English, "Bottom-Right"}, {Language::Chinese, "右下"}}},
        {"ingame_weakling", {{Language::English, "Weakling"}, {Language::Chinese, "弱小型"}}},
        {"ingame_standard", {{Language::English, "Standard"}, {Language::Chinese, "标准型"}}},
        {"ingame_special", {{Language::English, "Special"}, {Language::Chinese, "特殊型"}}},
        {"ingame_miniboss", {{Language::English, "MiniBoss"}, {Language::Chinese, "小头目"}}},
        {"ingame_boss", {{Language::English, "Boss"}, {Language::Chinese, "头目"}}},
        {"ingame_hp", {{Language::English, "HP"}, {Language::Chinese, "生命"}}},
        {"ingame_password", {{Language::English, "Password"}, {Language::Chinese, "密码"}}},
    };

    void Initialize() {
        // Default to English, will be set based on system language after font loading
        currentLanguage = Language::English;
    }
    
    void InitializeAfterFontLoad(bool chineseFontAvailable) {
        // After font loading, set default language based on system language and font availability
        if (chineseFontAvailable) {
            LANGID langId = GetUserDefaultUILanguage();
            WORD primaryLang = PRIMARYLANGID(langId);
            if (primaryLang == LANG_CHINESE) {
                currentLanguage = Language::Chinese;
            }
        }
    }

    Language GetCurrentLanguage() {
        return currentLanguage;
    }

    void SetLanguage(Language lang) {
        currentLanguage = lang;
    }

    const char* T(const char* key) {
        auto it = translations.find(key);
        if (it != translations.end()) {
            auto langIt = it->second.find(currentLanguage);
            if (langIt != it->second.end()) {
                return langIt->second.c_str();
            }
        }
        return key;
    }
}
