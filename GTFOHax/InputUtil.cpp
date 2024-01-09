#include <algorithm>
#include <array>
#include <string_view>
#include <Windows.h>
#include "../imgui/imgui.h"
#include "InputUtil.h"

/*
* Credits to: https://github.com/danielkrupinski/Osiris/blob/master/Source/InputUtil.h
*/


struct Key {
    constexpr Key(std::string_view name, int code) : name{ name }, code{ code } {  }

    std::string_view name;
    ImGuiKey code;
};

// indices must match KeyBind::KeyCode enum
static constexpr auto keyMap = std::to_array<Key>({
    { "'", ImGuiKey_Apostrophe},
    { ",", ImGuiKey_Comma },
    { "-", ImGuiKey_Minus },
    { ".", ImGuiKey_Period },
    { "/", ImGuiKey_Slash },
    { "0", ImGuiKey_0 },
    { "1", ImGuiKey_1 },
    { "2", ImGuiKey_2 },
    { "3", ImGuiKey_3 },
    { "4", ImGuiKey_4 },
    { "5", ImGuiKey_5 },
    { "6", ImGuiKey_6 },
    { "7", ImGuiKey_7 },
    { "8", ImGuiKey_8 },
    { "9", ImGuiKey_9 },
    { ";", ImGuiKey_Semicolon },
    { "=", ImGuiKey_Equal },
    { "A", ImGuiKey_A },
    { "ADD", ImGuiKey_KeypadAdd },
    { "B", ImGuiKey_B },
    { "BACKSPACE", ImGuiKey_Backspace },
    { "C", ImGuiKey_C },
    { "CAPSLOCK", ImGuiKey_CapsLock },
    { "D", ImGuiKey_D },
    { "DECIMAL", ImGuiKey_KeypadDecimal },
    { "DELETE", ImGuiKey_Delete },
    { "DIVIDE", ImGuiKey_KeypadDivide },
    { "DOWN", ImGuiKey_DownArrow },
    { "E", ImGuiKey_E },
    { "END", ImGuiKey_End },
    { "ENTER", ImGuiKey_Enter },
    { "F", ImGuiKey_F },
    { "F1", ImGuiKey_F1 },
    { "F10", ImGuiKey_F10 },
    { "F11", ImGuiKey_F11 },
    { "F12", ImGuiKey_F12 },
    { "F2", ImGuiKey_F2 },
    { "F3", ImGuiKey_F3 },
    { "F4", ImGuiKey_F4 },
    { "F5", ImGuiKey_F5 },
    { "F6", ImGuiKey_F6 },
    { "F7", ImGuiKey_F7 },
    { "F8", ImGuiKey_F8 },
    { "F9", ImGuiKey_F9 },
    { "G", ImGuiKey_G },
    { "H", ImGuiKey_H },
    { "HOME", ImGuiKey_Home },
    { "I", ImGuiKey_I },
    { "INSERT", ImGuiKey_Insert },
    { "J", ImGuiKey_J },
    { "K", ImGuiKey_K },
    { "L", ImGuiKey_L},
    { "LALT", ImGuiKey_LeftAlt },
    { "LCTRL", ImGuiKey_LeftCtrl },
    { "LEFT", ImGuiKey_LeftArrow },
    { "LSHIFT", ImGuiKey_LeftShift },
    { "M", ImGuiKey_M },
    { "MOUSE1", 0 },
    { "MOUSE2", 1 },
    { "MOUSE3", 2 },
    { "MOUSE4", 3 },
    { "MOUSE5", 4 },
    { "MULTIPLY", ImGuiKey_KeypadMultiply },
    { "MWHEEL_DOWN", 0 },
    { "MWHEEL_UP", 0 },
    { "N", ImGuiKey_N },
    { "NONE", 0 },
    { "NUMPAD_0", ImGuiKey_Keypad0 },
    { "NUMPAD_1", ImGuiKey_Keypad1 },
    { "NUMPAD_2", ImGuiKey_Keypad2 },
    { "NUMPAD_3", ImGuiKey_Keypad3 },
    { "NUMPAD_4", ImGuiKey_Keypad4 },
    { "NUMPAD_5", ImGuiKey_Keypad5 },
    { "NUMPAD_6", ImGuiKey_Keypad6 },
    { "NUMPAD_7", ImGuiKey_Keypad7 },
    { "NUMPAD_8", ImGuiKey_Keypad8 },
    { "NUMPAD_9", ImGuiKey_Keypad9 },
    { "O", ImGuiKey_O },
    { "P", ImGuiKey_P },
    { "PAGE_DOWN", ImGuiKey_PageDown },
    { "PAGE_UP", ImGuiKey_PageUp },
    { "Q", ImGuiKey_Q },
    { "R", ImGuiKey_R },
    { "RALT", ImGuiKey_RightAlt },
    { "RCTRL", ImGuiKey_RightCtrl },
    { "RIGHT", ImGuiKey_RightArrow },
    { "RSHIFT", ImGuiKey_RightShift },
    { "S", ImGuiKey_S },
    { "SPACE", ImGuiKey_Space },
    { "SUBTRACT", ImGuiKey_KeypadSubtract },
    { "T", ImGuiKey_T },
    { "TAB", ImGuiKey_Tab },
    { "U", ImGuiKey_U },
    { "UP", ImGuiKey_UpArrow },
    { "V", ImGuiKey_V },
    { "W", ImGuiKey_W },
    { "X", ImGuiKey_X },
    { "Y", ImGuiKey_Y },
    { "Z", ImGuiKey_Z },
    { "[", ImGuiKey_LeftBracket },
    { "\\", ImGuiKey_Backslash },
    { "]", ImGuiKey_RightBracket },
    { "`", ImGuiKey_GraveAccent }
    });

static_assert(keyMap.size() == KeyBind::MAX);
static_assert(std::ranges::is_sorted(keyMap, {}, & Key::name));

KeyBind::KeyBind(KeyCode keyCode) noexcept : keyCode{ static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE } {}

KeyBind::KeyBind(const char* keyName) noexcept
{
    if (const auto it = std::ranges::lower_bound(keyMap, keyName, {}, &Key::name); it != keyMap.end() && it->name == keyName)
        keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
    else
        keyCode = KeyCode::NONE;
}

const char* KeyBind::toString() const noexcept
{
    return keyMap[static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE].name.data();
}

bool KeyBind::isPressed() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseClicked(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyPressed(keyMap[keyCode].code, false);
}

bool KeyBind::isDown() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseDown(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyDown(keyMap[keyCode].code);
}

bool KeyBind::setToPressedKey() noexcept
{
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        keyCode = KeyCode::NONE;
        return true;
    }
    else if (ImGui::GetIO().MouseWheel < 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_DOWN;
        return true;
    }
    else if (ImGui::GetIO().MouseWheel > 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_UP;
        return true;
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); ++i) {
        if (ImGui::IsMouseClicked(i)) {
            keyCode = KeyCode(KeyCode::MOUSE1 + i);
            return true;
        }
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++i) {
        if (!ImGui::IsKeyPressed(ImGuiKey(i)))
            continue;

        if (const auto it = std::ranges::find(keyMap, i, &Key::code); it != keyMap.end()) {
            keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
            // Treat AltGr as RALT
            if (keyCode == KeyCode::LCTRL && ImGui::IsKeyPressed(keyMap[KeyCode::RALT].code))
                keyCode = KeyCode::RALT;
            return true;
        }
    }
    return false;
}

void KeyBindToggle::handleToggle() noexcept
{
    if (isPressed())
        toggledOn = !toggledOn;
}

