// happends in inspector

#pragma once
#include "GameWindow.hpp"
#include "imgui.h"
#include <stdexcept>
#include <type_traits>

static void DrawInspector(Object* object){
    ImGui::Begin("Inspector");
        if(!object){
            ImGui::Text("Select an object");
            ImGui::End();
            return;
        }

        ImGui::Text("%s", object->name.data());
        for(auto& c : object->components){
            c->DrawFileds();
        }
    ImGui::End();
}

