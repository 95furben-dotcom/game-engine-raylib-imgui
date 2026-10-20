#pragma once
#include "imgui.h"
#include "raylib.h"

static void DrawField(const char* name, float& v)   {
    ImGui::PushID(name);
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float MainlabelW  = ImGui::CalcTextSize(name).x;      
    const float itemW   = (ImGui::GetWindowWidth() - MainlabelW  - 4*spacing);


    ImGui::Text("%s", name);
    ImGui::SameLine(); 
    ImGui::SetNextItemWidth(itemW);
    ImGui::DragFloat("##", &v);
    ImGui::PopID();
}
static void DrawField(const char* name, int& v)     {
    ImGui::Text("%s", name);
    ImGui::SameLine(); 
    ImGui::DragInt("##", &v);
}
static void DrawField(const char* name, Vector2& v) {
    ImGui::PushID(name);

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float labelW  = ImGui::CalcTextSize("x:").x;      
    const float MainlabelW  = ImGui::CalcTextSize(name).x;      

    const float itemW   = (ImGui::GetWindowWidth() - MainlabelW - 2*labelW - 7*spacing)/2.0;


    ImGui::Text("%s", name);
    ImGui::SameLine(); 
    ImGui::Text("x:");
    ImGui::SameLine(); 
    ImGui::SetNextItemWidth(itemW);
    ImGui::DragFloat("##x", &v.x);
    ImGui::SameLine(); 
    ImGui::Text("y:");
    ImGui::SameLine(); 
    ImGui::SetNextItemWidth(itemW);
    ImGui::DragFloat("##y", &v.y);
    ImGui::PopID();

}

struct Component{
    virtual ~Component() = default;
    virtual void DrawFileds() = 0;   // "= 0" means every child must provide its own
};
struct CompTransform : virtual Component{
    Vector2 position{};
    Vector2 scale{};
    float rotation = 0;

    void DrawFileds(){
        DrawField("Position", position);
        DrawField("Scale", scale);
        DrawField("Rotation", rotation);
    }
};