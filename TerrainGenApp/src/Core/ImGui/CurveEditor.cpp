#include "CurveEditor.h"
#include <terraingen/types/math/Curve.h>

#include <cmath>
#include <algorithm>
#include <string>
#include <imgui_internal.h>

CurveEditor::CurveEditor(float minT, float maxT, float minV, float maxV)
	: minTime(minT), maxTime(maxT), minValue(minV), maxValue(maxV)
{
}

bool CurveEditor::Draw(const char* label, Curve& curve, const ImVec2& size)
{
    ImGui::PushID(label);

    ImGui::Text(label);


    // Draw the preview rectangle
    ImVec2 widgetPos = ImGui::GetCursorScreenPos();
    ImVec2 widgetSize = size;

    if (widgetSize.x <= 0.0f) widgetSize.x = ImGui::GetContentRegionAvail().x;

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Background
    drawList->AddRectFilled(widgetPos, ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y),
                            IM_COL32(30, 30, 30, 255));
    drawList->AddRect(widgetPos, ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y),
                      IM_COL32(100, 100, 100, 255));

    // Draw mini curve preview
    DrawCurvePreview(curve, drawList, widgetPos, widgetSize);

    // Make it clickable
    ImGui::InvisibleButton(label, widgetSize);
    bool clicked = ImGui::IsItemClicked();

    // Show popup on click
    if (clicked)
    {
        ImGui::OpenPopup("CurveWidgetPopup");
    }

    bool changed = false;

    // Draw the popup
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_Appearing);
    if (ImGui::BeginPopup("CurveWidgetPopup", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        changed = DrawFullEditor(label, curve);
        ImGui::EndPopup();
    }

    ImGui::PopID();
    return changed;
}

ImVec2 CurveEditor::TimeValueToScreen(float time, float value, const ImVec2& canvasPos, const ImVec2& canvasSize)
{
    float x = canvasPos.x + ((time - minTime) / (maxTime - minTime)) * canvasSize.x;
    float y = canvasPos.y + canvasSize.y - ((value - minValue) / (maxValue - minValue)) * canvasSize.y;
    return ImVec2(x, y);
}

void CurveEditor::ScreenToTimeValue(const ImVec2& screenPos, const ImVec2& canvasPos, const ImVec2& canvasSize, float& time, float& value)
{
    time = minTime + ((screenPos.x - canvasPos.x) / canvasSize.x) * (maxTime - minTime);
    value = minValue + ((canvasPos.y + canvasSize.y - screenPos.y) / canvasSize.y) * (maxValue - minValue);
}

void CurveEditor::DrawCurvePreview(Curve& curve, ImDrawList* drawList, const ImVec2& widgetPos, const ImVec2& widgetSize)
{
    auto& keys = curve.GetKeys();

    if (keys.size() < 2) return;

    int numSegments = 50;
    for (int i = 0; i < numSegments; i++)
    {
        float t0 = minTime + (maxTime - minTime) * (i / (float)numSegments);
        float t1 = minTime + (maxTime - minTime) * ((i + 1) / (float)numSegments);

        float v0 = curve.Evaluate(t0);
        float v1 = curve.Evaluate(t1);

        ImVec2 p0 = TimeValueToScreen(t0, v0, widgetPos, widgetSize);
        ImVec2 p1 = TimeValueToScreen(t1, v1, widgetPos, widgetSize);

        drawList->AddLine(p0, p1, IM_COL32(100, 255, 100, 255), 1.5f);
    }
}

bool CurveEditor::DrawFullEditor(const char* label, Curve& curve, const ImVec2& size)
{
    bool changed = false;

    ImGui::BeginGroup();
    ImGui::SeparatorText(label);

    auto& keys = curve.GetKeys();

    if (ImGui::SmallButton("Reset"))
    {
        keys.clear();
        keys.push_back({ 0.0f, 0.0f, 0.0f, 0.0f, false });
        keys.push_back({ 1.0f, 1.0f, 0.0f, 0.0f, false });
    }

    // Control inputs
    if (ImGui::BeginTable("EditorSettings", 8))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Min Time");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##Min Time", &minTime);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Max Time");
        ImGui::TableSetColumnIndex(3);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##Max Time", &maxTime);
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("Min Value");
        ImGui::TableSetColumnIndex(5);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##Min Value", &minValue);
        ImGui::TableSetColumnIndex(6);
        ImGui::Text("Max Value");
        ImGui::TableSetColumnIndex(7);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##Max Value", &maxValue);

        ImGui::EndTable();
    }

    // Display selected key values with padding when not present
    int selectedCount = 0;
    int selectedIndex = -1;
    for (int i = 0; i < keys.size(); i++)
    {
        if (keys[i].selected)
        {
            selectedCount++;
            selectedIndex = i;
        }
    }

    if (selectedCount == 1 && selectedIndex >= 0)
    {
        if (ImGui::BeginTable("Key Settings", 9))
        {
            ImVec2 maxTextSize = ImGui::CalcTextSize("Tangent");
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, maxTextSize.x + ImGui::GetStyle().FramePadding.x * 2.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, maxTextSize.x + ImGui::GetStyle().FramePadding.x * 2.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, maxTextSize.x + ImGui::GetStyle().FramePadding.x * 2.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, maxTextSize.x + ImGui::GetStyle().FramePadding.x * 2.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, maxTextSize.x + ImGui::GetStyle().FramePadding.x * 2.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 60.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Key: %i", selectedIndex + 1);

            float tempTime = keys[selectedIndex].time;
            float tempValue = keys[selectedIndex].value;
            float tempInTangent = keys[selectedIndex].inTangent;
            float tempOutTangent = keys[selectedIndex].outTangent;

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Time");
            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputFloat("##time", &tempTime, 0.0f, 0.0f, "%.3f"))
            {
                keys[selectedIndex].time = std::clamp(tempTime, minTime, maxTime);
                std::sort(keys.begin(), keys.end(), [](const CurveKey& a, const CurveKey& b) { return a.time < b.time; });
            }
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Value");
            ImGui::TableSetColumnIndex(4);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputFloat("##value", &tempValue, 0.0f, 0.0f, "%.3f"))
            {
                keys[selectedIndex].value = std::clamp(tempValue, minValue, maxValue);
            }

            ImGui::TableSetColumnIndex(5);
            ImGui::Text("In Tan");
            ImGui::TableSetColumnIndex(6);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputFloat("##intangent", &tempInTangent, 0.0f, 0.0f, "%.3f"))
            {
                keys[selectedIndex].inTangent = tempInTangent;
                keys[selectedIndex].outTangent = -tempInTangent; // Mirror inverse
            }
            ImGui::TableSetColumnIndex(7);
            ImGui::Text("Out Tan");
            ImGui::TableSetColumnIndex(8);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputFloat("##outtangent", &tempOutTangent, 0.0f, 0.0f, "%.3f"))
            {
                keys[selectedIndex].outTangent = tempOutTangent;
                keys[selectedIndex].inTangent = -tempOutTangent; // Mirror inverse
            }

            ImGui::EndTable();
        }
    }
    else if (selectedCount > 1)
    {
        ImGui::Text("Multiple keys selected (%d)", selectedCount);
    }
    else
    {
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetFrameHeight() + 4.0f));
    }

    ImGui::Dummy(ImVec2(0, 10.0f));

    // Ensure valid ranges
    if (maxTime <= minTime) maxTime = minTime + 1.0f;
    if (maxValue <= minValue) maxValue = minValue + 1.0f;

    // Main canvas
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = size;
    if (canvasSize.x <= 0) canvasSize.x = ImGui::GetContentRegionAvail().x;
    if (canvasSize.y <= 0) canvasSize.y = ImGui::GetContentRegionAvail().y;

    if (canvasSize.y < 200) canvasSize.y = 200;

    // Draw background
    ImGui::InvisibleButton("##canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft |
                           ImGuiButtonFlags_MouseButtonRight |
                           ImGuiButtonFlags_MouseButtonMiddle);
    bool isHovered = ImGui::IsItemHovered();

    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x , canvasPos.y + canvasSize.y),
                            IM_COL32(40, 40, 40, 255));
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                      IM_COL32(100, 100, 100, 255));

    // Draw grid
    DrawGrid(drawList, canvasPos, canvasSize);

    // Handle input
    changed = HandleInput(curve, canvasPos, canvasSize, isHovered);

    // Draw curve
    DrawCurve(curve, drawList, canvasPos, canvasSize);

    // Draw keys and handles
    DrawKeys(curve, drawList, canvasPos, canvasSize);

    ImGui::EndGroup();

    return changed;
}

void CurveEditor::DrawGrid(ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize)
{
    int numVerticalLines = 10;
    for (int i = 0; i <= numVerticalLines; i++)
    {
        float t = minTime + (maxTime - minTime) * (i / (float)numVerticalLines);
        ImVec2 pos = TimeValueToScreen(t, minValue, canvasPos, canvasSize);
        drawList->AddLine(ImVec2(pos.x, canvasPos.y), ImVec2(pos.x, canvasPos.y + canvasSize.y),
                          IM_COL32(60, 60, 60, 255));

        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", t);
        drawList->AddText(ImVec2(pos.x + 2, canvasPos.y + canvasSize.y - 15), IM_COL32(150, 150, 150, 255), buf);
    }

    int numHorizontalLines = 10;
    for (int i = 0; i <= numHorizontalLines; i++)
    {
        float v = minValue + (maxValue - minValue) * (i / (float)numHorizontalLines);
        ImVec2 pos = TimeValueToScreen(minTime, v, canvasPos, canvasSize);
        drawList->AddLine(ImVec2(canvasPos.x, pos.y), ImVec2(canvasPos.x + canvasSize.x, pos.y),
                          IM_COL32(60, 60, 60, 255));

        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", v);
        drawList->AddText(ImVec2(canvasPos.x + 2, pos.y - 15), IM_COL32(150, 150, 150, 255), buf);
    }
}

bool CurveEditor::HandleInput(Curve& curve, const ImVec2& canvasPos, const ImVec2& canvasSize, bool isHovered)
{
    bool changed = false;

    ImGuiIO& io = ImGui::GetIO();

    auto& keys = curve.GetKeys();

    // Handle double click to add key
    if (isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        float time, value;
        ScreenToTimeValue(io.MousePos, canvasPos, canvasSize, time, value);
        time = std::clamp(time, minTime, maxTime);
        value = std::clamp(value, minValue, maxValue);

        CurveKey newKey = { time, value, 0.0f, 0.0f, false };
        curve.AddKey(newKey);

        changed = true;
    }

    // Handle key selection, handle dragging, and deselection
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered)
    {
        draggedKey = -1;
        draggedHandle = -1;
        bool clickedOnSomething = false;

        // Check if clicking on a handle first
        for (int i = 0; i < keys.size(); i++)
        {
            if (!keys[i].selected) continue;

            ImVec2 keyPos = TimeValueToScreen(keys[i].time, keys[i].value, canvasPos, canvasSize);
            ImVec2 inHandle = ImVec2(keyPos.x - HANDLE_LENGTH, keyPos.y + keys[i].inTangent * HANDLE_LENGTH);
            ImVec2 outHandle = ImVec2(keyPos.x + HANDLE_LENGTH, keyPos.y - keys[i].outTangent * HANDLE_LENGTH);

            float distIn = sqrtf(powf(io.MousePos.x - inHandle.x, 2) + powf(io.MousePos.y - inHandle.y, 2));
            float distOut = sqrtf(powf(io.MousePos.x - outHandle.x, 2) + powf(io.MousePos.y - outHandle.y, 2));

            if (distIn < 8.0f)
            {
                draggedKey = i;
                draggedHandle = 0;
                clickedOnSomething = true;
                break;
            }
            else if (distOut < 8.0f)
            {
                draggedKey = i;
                draggedHandle = 1;
                clickedOnSomething = true;
                break;
            }
        }

        // Check if clicking on a key
        if (!clickedOnSomething)
        {
            for (int i = 0; i < keys.size(); i++)
            {
                ImVec2 keyPos = TimeValueToScreen(keys[i].time, keys[i].value, canvasPos, canvasSize);
                float dist = sqrtf(powf(io.MousePos.x - keyPos.x, 2) + powf(io.MousePos.y - keyPos.y, 2));

                if (dist < 8.0f)
                {
                    if (!io.KeyCtrl)
                    {
                        for (auto& k : keys) k.selected = false;
                    }
                    keys[i].selected = true;
                    draggedKey = i;
                    clickedOnSomething = true;
                    break;
                }
            }
        }

        // Deselect all if clicked on empty canvas
        if (!clickedOnSomething)
        {
            for (auto& k : keys) k.selected = false;
        }
    }

    // Handle middle mouse button drag for selected keys
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) && isHovered)
    {
        for (int i = 0; i < keys.size(); i++)
        {
            if (keys[i].selected)
            {
                middleMouseDrag = true;
                break;
            }
        }
    }

    // Drag keys or handles
    if ((ImGui::IsMouseDragging(ImGuiMouseButton_Left) && draggedKey >= 0) ||
        (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) && middleMouseDrag))
    {
        ImVec2 delta = io.MouseDelta;
        float timeDelta = (delta.x / canvasSize.x) * (maxTime - minTime);
        float valueDelta = -(delta.y / canvasSize.y) * (maxValue - minValue);

        if (draggedHandle >= 0 && draggedKey >= 0)
        {
            float tangentDelta = -delta.y / HANDLE_LENGTH;

            if (draggedHandle == 0)
            {
                keys[draggedKey].inTangent += -tangentDelta;
                keys[draggedKey].outTangent = keys[draggedKey].inTangent;

                changed = true;
            }
            else if (draggedHandle == 1)
            {
                keys[draggedKey].outTangent += tangentDelta;
                keys[draggedKey].inTangent = keys[draggedKey].outTangent;

                changed = true;
            }
        }
        else if (draggedKey >= 0)
        {
            keys[draggedKey].time = std::clamp(keys[draggedKey].time + timeDelta, minTime, maxTime);
            keys[draggedKey].value = std::clamp(keys[draggedKey].value + valueDelta, minValue, maxValue);

            changed = true;
        }
        else
        {
            for (auto& key : keys)
            {
                if (key.selected)
                {
                    key.time = std::clamp(key.time + timeDelta, minTime, maxTime);
                    key.value = std::clamp(key.value + valueDelta, minValue, maxValue);
                    changed = true;
                }
            }
        }

        std::sort(keys.begin(), keys.end(), [](const CurveKey& a, const CurveKey& b) { return a.time < b.time; });
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        draggedKey = -1;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
    {
        middleMouseDrag = false;
    }

    // Delete key with right click
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && isHovered)
    {
        for (unsigned int i = keys.size() - 1; i > 0; i--)
        {
            ImVec2 keyPos = TimeValueToScreen(keys[i].time, keys[i].value, canvasPos, canvasSize);
            float dist = sqrtf(powf(io.MousePos.x - keyPos.x, 2) + powf(io.MousePos.y - keyPos.y, 2));

            if (dist < 8.0f)
            {
                keys.erase(keys.begin() + i);
                changed = true;
                break;
            }
        }
    }

    // Delete selected keys with Delete key
    if (ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        keys.erase(std::remove_if(keys.begin(), keys.end(), [](const CurveKey& k) { return k.selected; }), keys.end());
        changed = true;
    }

    return changed;
}

void CurveEditor::DrawCurve(Curve& curve, ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize)
{
    auto& keys = curve.GetKeys();

    if (keys.size() < 2) return;

    int numSegments = 100;
    for (int i = 0; i < numSegments; i++)
    {
        float t0 = minTime + (maxTime - minTime) * (i / (float)numSegments);
        float t1 = minTime + (maxTime - minTime) * ((i + 1) / (float)numSegments);

        float v0 = curve.Evaluate(t0);
        float v1 = curve.Evaluate(t1);

        ImVec2 p0 = TimeValueToScreen(t0, v0, canvasPos, canvasSize);
        ImVec2 p1 = TimeValueToScreen(t1, v1, canvasPos, canvasSize);

        drawList->AddLine(p0, p1, IM_COL32(100, 255, 100, 255), 2.0f);
    }
}

void CurveEditor::DrawKeys(Curve& curve, ImDrawList* drawList, const ImVec2& canvasPos, const ImVec2& canvasSize)
{
    ImGuiIO& io = ImGui::GetIO();

    auto& keys = curve.GetKeys();

    for (int i = 0; i < keys.size(); i++)
    {
        const auto& key = keys[i];
        ImVec2 keyPos = TimeValueToScreen(key.time, key.value, canvasPos, canvasSize);

        // Draw tangent handles
        ImVec2 inHandle = ImVec2(keyPos.x - HANDLE_LENGTH, keyPos.y + key.inTangent * HANDLE_LENGTH);
        ImVec2 outHandle = ImVec2(keyPos.x + HANDLE_LENGTH, keyPos.y - key.outTangent * HANDLE_LENGTH);

        if (key.selected)
        {
            drawList->AddLine(keyPos, inHandle, IM_COL32(255, 255, 0, 150), 1.5f);
            drawList->AddLine(keyPos, outHandle, IM_COL32(255, 255, 0, 150), 1.5f);
            drawList->AddCircleFilled(inHandle, 4.0f, IM_COL32(255, 255, 0, 255));
            drawList->AddCircleFilled(outHandle, 4.0f, IM_COL32(255, 255, 0, 255));
        }

        // Draw key
        ImU32 keyColor = key.selected ? IM_COL32(255, 200, 0, 255) : IM_COL32(255, 255, 255, 255);
        drawList->AddCircleFilled(keyPos, 6.0f, keyColor);
        drawList->AddCircle(keyPos, 6.0f, IM_COL32(0, 0, 0, 255), 0, 2.0f);

        // Show tooltip when hovering over key
        float dist = sqrtf(powf(io.MousePos.x - keyPos.x, 2) + powf(io.MousePos.y - keyPos.y, 2));
        if (dist < 8.0f)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Time: %.3f", key.time);
            ImGui::Text("Value: %.3f", key.value);
            ImGui::Text("In Tangent: %.3f", key.inTangent);
            ImGui::Text("Out Tangent: %.3f", key.outTangent);
            ImGui::EndTooltip();
        }
    }
}