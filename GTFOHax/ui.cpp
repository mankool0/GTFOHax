#include "../imgui/imgui_internal.h"

#include "ui.h"
#include "InputUtil.h"

/*
* Credits to: https://github.com/danielkrupinski/Osiris/blob/master/Source/imguiCustom.cpp
*/

void ImGui::Hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size) noexcept
{
    const auto id = GetID(label);
    PushID(label);

    TextUnformatted(label);
    SameLine(samelineOffset);

    if (GetActiveID() == id || key.isWaiting())
    {
        if (!key.isWaiting())
            key.toggleWaiting();
        PushStyleColor(ImGuiCol_Button, GetColorU32(ImGuiCol_ButtonActive));
        Button("...", size);
        PopStyleColor();

        GetCurrentContext()->ActiveIdAllowOverlap = true;
        if ((!IsItemHovered() && GetIO().MouseClicked[0]) || key.setToPressedKey())
        {
            ClearActiveID();
            key.toggleWaiting();
        }
    }
    else if (Button(key.toString(), size))
    {
        SetActiveID(id, GetCurrentWindow());
    }

    PopID();
}
