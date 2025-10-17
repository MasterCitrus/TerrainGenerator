#pragma once


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <sstream>

class Curve;

class CurveEditor
{
public:
    CurveEditor() = default;
    CurveEditor(float minT, float maxT, float minV, float maxV);

    bool Draw(const char* label, Curve& curve, const ImVec2& size = ImVec2(0, 50));

private:
    ImVec2 TimeValueToScreen(float time, float value, const ImVec2& canvasPos, const ImVec2& canvasSize);
    void ScreenToTimeValue(const ImVec2& screenPos, const ImVec2& canvasPos, const ImVec2& canvasSize, float& time, float& value);

    void DrawCurvePreview(Curve& curve, ImDrawList* drawList, const ImVec2& widgetPos, const ImVec2& widgetSize);
    bool DrawFullEditor(const char* label, Curve& curve, const ImVec2& size = ImVec2(0, 0));

    void DrawGrid(ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize);
    bool HandleInput(Curve& curve, const ImVec2& canvasPos, const ImVec2& canvasSize, bool isHovered);
    void DrawCurve(Curve& curve, ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize);
    void DrawKeys(Curve& curve, ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize);

private:
    float minTime = 0.0f;
    float maxTime = 1.0f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    int draggedKey = -1;
    int draggedHandle = -1; // 0 = in tangent, 1 = out tangent
    bool middleMouseDrag = false;
    static constexpr float HANDLE_LENGTH = 50.0f;
};

static inline std::string fmtf(float v, int prec = 3)
{
    std::ostringstream ss; ss.setf(std::ios::fixed); ss.precision(prec); ss << v; return ss.str();
}

#undef IMGUI_DEFINE_MATH_OPERATORS
