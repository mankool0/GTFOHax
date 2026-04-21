#include "i18n.h"
#include <windows.h>

#if ENABLE_CHINESE
#define ZH(s) , {Language::Chinese, (s)}
#else
#define ZH(s)
#endif

namespace I18N {
    static Language currentLanguage = Language::English;

    const char* LanguageNames[] = {
        "English",
#if ENABLE_CHINESE
        "中文", // Chinese
#endif
    };

    const int LanguageCount = sizeof(LanguageNames) / sizeof(LanguageNames[0]);

    static std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations = {
        {"tab_player", {{Language::English, "Player"} ZH("玩家")}},
        {"tab_esp", {{Language::English, "ESP"} ZH("透视")}},
        {"tab_aimbot", {{Language::English, "Aimbot"} ZH("自瞄")}},
        {"tab_misc", {{Language::English, "Misc"} ZH("杂项")}},
        {"tab_config", {{Language::English, "Config"} ZH("配置")}},
        {"player_godmode", {{Language::English, "Godmode"} ZH("无敌模式")}},
        {"player_infinite_ammo", {{Language::English, "Infinite Ammo"} ZH("无限弹药")}},
        {"player_gluegun_no_cooldown", {{Language::English, "Gluegun No Cooldown"} ZH("胶枪无冷却")}},
        {"player_instant_hack", {{Language::English, "Instant Hack"} ZH("瞬间破解")}},
        {"player_full_auto", {{Language::English, "Full Auto"} ZH("全自动射击")}},
        {"player_no_recoil", {{Language::English, "No Recoil"} ZH("无后坐力")}},
        {"player_no_spread", {{Language::English, "No Spread"} ZH("无扩散")}},
        {"player_no_shake", {{Language::English, "No Shake"} ZH("无抖动")}},
        {"player_no_fog", {{Language::English, "No Fog"} ZH("无雾")}},
        {"player_give_health", {{Language::English, "Give Health"} ZH("给予生命")}},
        {"player_give_disinfection", {{Language::English, "Give Disinfection"} ZH("给予消毒")}},
        {"player_give_ammo", {{Language::English, "Give Ammo"} ZH("给予弹药")}},
        {"player_auto_self_revive", {{Language::English, "Auto Self Revive"} ZH("自动自救")}},
        {"player_self_revive", {{Language::English, "Self Revive"} ZH("自救")}},
        {"player_full_hp_revive", {{Language::English, "Full HP on Revive"} ZH("复活满血")}},
        {"esp_world_esp", {{Language::English, "World ESP"} ZH("世界透视")}},
        {"esp_enemy_esp", {{Language::English, "Enemy ESP"} ZH("敌人透视")}},
        {"esp_world", {{Language::English, "World"} ZH("世界")}},
        {"esp_enemy", {{Language::English, "Enemy"} ZH("敌人")}},
        {"esp_enabled", {{Language::English, "Enabled"} ZH("启用")}},
        {"esp_boxes", {{Language::English, "Boxes"} ZH("方框")}},
        {"esp_2d_boxes", {{Language::English, "2D Boxes"} ZH("2D方框")}},
        {"esp_healthbar", {{Language::English, "Healthbar"} ZH("血条")}},
        {"esp_healthbar_text", {{Language::English, "Healthbar Text"} ZH("血条文字")}},
        {"esp_healthbar_full_text", {{Language::English, "Healthbar Full Health Text"} ZH("满血显示文字")}},
        {"esp_bar_thickness", {{Language::English, "Bar Thickness"} ZH("血条厚度")}},
        {"esp_enemy_info", {{Language::English, "Enemy Info"} ZH("敌人信息")}},
        {"esp_name", {{Language::English, "Name"} ZH("名称")}},
        {"esp_type", {{Language::English, "Type"} ZH("类型")}},
        {"esp_health", {{Language::English, "Health"} ZH("生命值")}},
        {"esp_distance", {{Language::English, "Distance"} ZH("距离")}},
        {"esp_skeleton", {{Language::English, "Skeleton"} ZH("骨骼")}},
        {"esp_render_distance", {{Language::English, "Render Distance"} ZH("渲染距离")}},
        {"esp_line_thickness", {{Language::English, "Line Thickness"} ZH("线条粗细")}},
        {"esp_visible", {{Language::English, "Visible"} ZH("可见")}},
        {"esp_non_visible", {{Language::English, "Non-Visible"} ZH("不可见")}},
        {"esp_locked", {{Language::English, "LOCKED"} ZH("锁定")}},
        {"aimbot_enemy_aimbot", {{Language::English, "Enemy Aimbot"} ZH("敌人自瞄")}},
        {"aimbot_on_hold", {{Language::English, "On Hold"} ZH("按住生效")}},
        {"aimbot_silent_aim", {{Language::English, "Silent Aim"} ZH("静默自瞄")}},
        {"aimbot_magic_bullet", {{Language::English, "Magic Bullet"} ZH("魔法子弹")}},
        {"aimbot_magic_bullet_direction", {{Language::English, "Direction"} ZH("射击方向")}},
        {"aimbot_magic_bullet_dir_above", {{Language::English, "From Above"} ZH("从上方")}},
        {"aimbot_magic_bullet_dir_front", {{Language::English, "From Front"} ZH("从正面")}},
        {"aimbot_magic_bullet_dir_behind", {{Language::English, "From Behind"} ZH("从后方")}},
        {"aimbot_magic_bullet_dir_player", {{Language::English, "From Player"} ZH("从玩家")}},
        {"aimbot_magic_bullet_offset", {{Language::English, "Offset"} ZH("偏移距离")}},
        {"aimbot_hit_ghost", {{Language::English, "Hit Ghost"} ZH("命中残影")}},
        {"aimbot_hit_ghost_duration", {{Language::English, "Duration"} ZH("持续时间")}},
        {"aimbot_hit_ghost_thickness", {{Language::English, "Thickness"} ZH("线条粗细")}},
        {"aimbot_bullet_ray", {{Language::English, "Bullet Ray"} ZH("子弹射线")}},
        {"aimbot_bullet_ray_duration", {{Language::English, "Duration"} ZH("持续时间")}},
        {"aimbot_bullet_ray_thickness", {{Language::English, "Thickness"} ZH("线条粗细")}},
        {"aimbot_target_highlight", {{Language::English, "Highlight Target"} ZH("高亮锁定目标")}},
        {"aimbot_target_marker", {{Language::English, "Show Marker"} ZH("显示标记")}},
        {"aimbot_visible_only", {{Language::English, "Visible Only"} ZH("仅可见目标")}},
        {"aimbot_aim_at_armor", {{Language::English, "Aim At Armor"} ZH("瞄准护甲")}},
        {"aimbot_aim_distance", {{Language::English, "Aim Distance"} ZH("自瞄距离")}},
        {"aimbot_smoothing", {{Language::English, "Smoothing"} ZH("平滑度")}},
        {"aimbot_render_fov", {{Language::English, "Render FOV"} ZH("显示视野")}},
        {"aimbot_aim_priority", {{Language::English, "Aim Priority"} ZH("瞄准优先级")}},
        {"aimbot_priority_health", {{Language::English, "Health"} ZH("生命值")}},
        {"aimbot_priority_distance", {{Language::English, "Distance"} ZH("距离")}},
        {"aimbot_priority_fov", {{Language::English, "FOV"} ZH("视野")}},
        {"player_full_bright", {{Language::English, "Full Bright"} ZH("全亮")}},
        {"player_full_bright_range", {{Language::English, "Range"} ZH("范围")}},
        {"player_full_bright_intensity", {{Language::English, "Intensity"} ZH("强度")}},
        {"player_full_bright_angle", {{Language::English, "Angle"} ZH("角度")}},
        {"player_full_bright_color", {{Language::English, "Color"} ZH("颜色")}},
        {"player_damage_multipliers", {{Language::English, "Damage Multipliers"} ZH("伤害倍率")}},
        {"player_damage_enemy", {{Language::English, "Enemy Damage"} ZH("对敌伤害")}},
        {"player_damage_received_self", {{Language::English, "Received (Self)"} ZH("受到伤害(自身)")}},
        {"player_damage_received_team", {{Language::English, "Received (Team/Bot)"} ZH("受到伤害(队友/机器人)")}},
        {"player_force_weakspot", {{Language::English, "Force Weakspot"} ZH("强制弱点")}},
        {"player_turret_damage", {{Language::English, "Turret Damage"} ZH("炮台伤害")}},
        {"player_turret_local_only", {{Language::English, "My Turrets Only"} ZH("仅自己的炮台")}},
        {"misc_toggle_menu_key", {{Language::English, "Toggle Menu Key"} ZH("菜单开关键")}},
        {"misc_unload_menu", {{Language::English, "Unload Menu"} ZH("卸载菜单")}},
        {"misc_key", {{Language::English, "Key"} ZH("按键")}},
        {"misc_watermark", {{Language::English, "Watermark"} ZH("水印")}},
        {"misc_location", {{Language::English, "Location"} ZH("位置")}},
        {"misc_spawn_enemy", {{Language::English, "Spawn Enemy"} ZH("生成敌人")}},
        {"misc_language", {{Language::English, "Language"} ZH("语言")}},
        {"config_save", {{Language::English, "Save"} ZH("保存")}},
        {"config_load", {{Language::English, "Load"} ZH("加载")}},
        {"corner_top_left", {{Language::English, "Top-Left"} ZH("左上")}},
        {"corner_top_right", {{Language::English, "Top-Right"} ZH("右上")}},
        {"corner_bottom_left", {{Language::English, "Bottom-Left"} ZH("左下")}},
        {"corner_bottom_right", {{Language::English, "Bottom-Right"} ZH("右下")}},
        {"ingame_weakling", {{Language::English, "Weakling"} ZH("弱小型")}},
        {"ingame_standard", {{Language::English, "Standard"} ZH("标准型")}},
        {"ingame_special", {{Language::English, "Special"} ZH("特殊型")}},
        {"ingame_miniboss", {{Language::English, "MiniBoss"} ZH("小头目")}},
        {"ingame_boss", {{Language::English, "Boss"} ZH("头目")}},
        {"ingame_hp", {{Language::English, "HP"} ZH("生命")}},
        {"ingame_password", {{Language::English, "Password"} ZH("密码")}},
    };

    void Initialize() {
        currentLanguage = Language::English;
    }

    void InitializeAfterFontLoad(bool chineseFontAvailable) {
#if ENABLE_CHINESE
        if (chineseFontAvailable) {
            LANGID langId = GetUserDefaultUILanguage();
            WORD primaryLang = PRIMARYLANGID(langId);
            if (primaryLang == LANG_CHINESE) {
                currentLanguage = Language::Chinese;
            }
        }
#endif
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
