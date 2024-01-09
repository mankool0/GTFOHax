#pragma once
#include "../imgui/imgui.h"

class KeyBind;

namespace ImGui
{
    void Hotkey(const char* label, KeyBind& key, float samelineOffset = 0.0f, const ImVec2& size = { 100.0f, 0.0f }) noexcept;
}