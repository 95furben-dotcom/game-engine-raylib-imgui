#include "Windows/Explorer.hpp"
#include "Windows/GameWindow.hpp"
#include "Windows/Inspector.hpp"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
#include <imgui_internal.h>
#include <vector>

void SetupDocking(){
    ImGuiID root = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    
    ImGui::DockBuilderRemoveNode(root);
    ImGui::DockBuilderAddNode(root, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(root, ImGui::GetMainViewport()->Size);
    // ... splits and DockWindow calls ...
    ImGui::DockBuilderFinish(root);


    ImGuiID left = ImGui::DockBuilderSplitNode(root, ImGuiDir_Left, 0.2f, nullptr, &root);
    ImGuiID right = ImGui::DockBuilderSplitNode(root, ImGuiDir_Right, 0.25, nullptr, &root);
    ImGuiID top = ImGui::DockBuilderSplitNode(root, ImGuiDir_Up, 0.5f, nullptr, &root);

    ImGui::DockBuilderDockWindow("Hierarchy", left);
    ImGui::DockBuilderDockWindow("Inspector", right);
    ImGui::DockBuilderDockWindow("Scene", top);
    ImGui::DockBuilderDockWindow("Explorer", root);

}

int main(){
    SetTraceLogLevel(TraceLogLevel::LOG_WARNING);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1450, 720, "SimpleEngine");
    SetTargetFPS(60);
    // SetExitKey(0);

    // imGui
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    RenderTexture2D gameView = LoadRenderTexture(1280, 720);

    
    
    std::vector<Object*> objects = {new Object("Object1"),new Object("Object2")};
    int selected = -1;
    
    Explorer explorer("/Users/benjaminflynnfurnes/Documents/CppProjects/Raylib/GameEngine/");
    while(!WindowShouldClose()){
        BeginTextureMode(gameView);
            DrawGameWindow(gameView, objects);
        EndTextureMode();
        
        BeginDrawing();
            rlImGuiBegin();
                static bool layoutBuilt = false;
                if (!layoutBuilt) {
                    layoutBuilt = true;
                    SetupDocking();
                }

                ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

                if(ImGui::Begin("Hierarchy")){
                    ImGui::Text("This is the Hierarchy");
                    for(int i = 0; i < objects.size(); i++){
                        if(ImGui::Selectable(objects[i]->name.data(), selected == i))
                            selected = i;

                    }
                }
                ImGui::End();

                if(ImGui::Begin("Scene")){
                    ImVec2 size = ImGui::GetContentRegionAvail(); 
                    rlImGuiImageRenderTextureFit(&gameView, true);
                }
                ImGui::End();

                DrawInspector((selected != -1) ? objects[selected] : nullptr);
                explorer.DrawExplorer();
                
                auto releasedFile = explorer.DrawSelectedFile();
                if(releasedFile){
                    
                }
                
            rlImGuiEnd();
        EndDrawing();
        
    }
    rlImGuiShutdown();
    UnloadRenderTexture(gameView);
    CloseWindow();
}