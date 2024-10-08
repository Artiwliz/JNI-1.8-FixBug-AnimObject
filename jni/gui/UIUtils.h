#pragma once

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"

#include <string>

ImColor RenderTextAndGetLastColor(ImFont *font, const float font_size, uint8_t outline, ImVec2 pos, ImColor col, const char* szStr);
ImVec2 CalcTextSizeWithoutTags(ImFont *font, const float font_size, const char* szStr);

class UIUtils {
    friend class CGUI;
public:
    static bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
    static void drawText(const char* fmt, ...);
    static void drawText(ImVec2 pos, ImColor col, const char* szStr, const float font_size = 0.0f);
    static void drawText(ImVec2 pos, char* utf8string, uint32_t dwColor);
    static void FilterColors(char* szStr);

    static ImVec2 CalcTextSizeWithoutTags(char* szStr, float font_size = 0.0f);
    static std::string removeColorTags(std::string line);

    static void CreateCircleProgressBar(ImGuiWindow* window, ImVec2 pos, ImColor color, int value, float radius, float thickness);
private:


};