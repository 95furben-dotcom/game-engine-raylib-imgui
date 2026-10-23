#pragma once
#include "imgui.h"
#include "raylib.h"

namespace field {

struct Layout {
    float spacing, labelW, itemW, vectorW, singleW;
};

inline Layout GetLayout() {
    Layout l;
    l.spacing = ImGui::GetStyle().ItemSpacing.x;
    l.labelW  = ImGui::CalcTextSize("x:").x;
    l.itemW   = 70.0f;
    // label + item + label + item, with 3 gaps between them
    l.vectorW = l.itemW * 2 + l.labelW * 2 + l.spacing * 3;
    l.singleW = l.vectorW;   // float field lines up with the vector fields
    return l;
}

inline void Draw(const char* name, float& v) {
    const Layout l = GetLayout();
    ImGui::PushID(name);
    ImGui::TextUnformatted(name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - l.singleW - l.spacing);
    ImGui::SetNextItemWidth(l.singleW);
    ImGui::DragFloat("##v", &v);
    ImGui::PopID();
}

inline void Draw(const char* name, int& v) {
    const Layout l = GetLayout();
    ImGui::PushID(name);                       // was missing: "##" IDs would collide
    ImGui::TextUnformatted(name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - l.singleW - l.spacing);
    ImGui::SetNextItemWidth(l.singleW);
    ImGui::DragInt("##v", &v);
    ImGui::PopID();
}

inline void Draw(const char* name, Vector2& v) {
    const Layout l = GetLayout();
    ImGui::PushID(name);
    ImGui::TextUnformatted(name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - l.vectorW - l.spacing);

    ImGui::TextUnformatted("x:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(l.itemW);
    ImGui::DragFloat("##x", &v.x);

    ImGui::SameLine();
    ImGui::TextUnformatted("y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(l.itemW);
    ImGui::DragFloat("##y", &v.y);
    ImGui::PopID();
}

} // namespace field

struct Component {
    virtual ~Component() = default;
    virtual void DrawFields() = 0;
};

struct CompTransform : Component {         
    Vector2 position{0, 0};
    Vector2 scale{1, 1};               
    float rotation = 0;

    void DrawFields() override {
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        ImGui::BeginChild("Transform",
            {ImGui::GetWindowSize().x - spacing * 2.0f, 0},
            ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
            field::Draw("Position", position);
            field::Draw("Scale", scale);
            field::Draw("Rotation", rotation);
        ImGui::EndChild();
    }
};