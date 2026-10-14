#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "raylib.h"
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

#include <filesystem>
#include <vector>
#include "rlImGui.h"
#include <sstream>

#include <imgui_stdlib.h>
#include "../imguihelpers.hpp"
namespace fs = std::filesystem;

struct Explorer{
    Texture2D folderTex = LoadTexture("Resources/folder.png");
    Texture2D fileTex = LoadTexture("Resources/file.png");
    Texture2D spyglassTex = LoadTexture("Resources/spyglass.png");
    Texture2D copyTex = LoadTexture("Resources/copy.png");
    float childsize = 150;
    const float yxiconRatio = 0.7; 
    std::string path;

    std::string searchbarContent;

    fs::directory_entry* selectedFile = nullptr;
    ImVec2 fileOffsetOnSelect = {};
    Explorer(std::string path) : path(path){}

    static std::vector<std::string> split(std::string s, char c){
        std::stringstream test(s);
        std::string segment;
        std::vector<std::string> seglist;

        while(std::getline(test, segment, c))
        {
            seglist.push_back(segment);
        }
        return seglist;
    }
      static std::string join(std::vector<std::string> segs, char c){
        std::string s;
        for (int i = 0; i < segs.size(); i++) {
            s += segs[i];
            s += c;
        }
        return s;
    }


    void OpenInVSCode(const std::string& path) {
        std::string cmd = "open -a \"Visual Studio Code\" \"" + path + "\"";
        system(cmd.c_str());
    }

    void RenderPath(){
        std::vector<std::string> segs = split(path, '/');
        for (int i = 0; i < segs.size(); i++) {
            ImGui::SameLine(0);
            std::string s = segs[i] + "/";
            if(ImGui::Selectable(s.c_str(), false, 0,ImGui::CalcTextSize(s.c_str()))){
                segs.resize(i+1);
                path = join(segs, '/');
                searchbarContent = "";
            }
        }
    }

    bool stringInString(std::string a, std::string b){
        for(char& c : a)
            c = tolower(c);
        for(char& c : b)
            c = tolower(c);
        return a.contains(b);
    }

    void DrawFileHitbox(fs::directory_entry entry, const float iconSize) {
        ImVec2 position = {(ImGui::GetWindowWidth() - iconSize) / 2, ImGui::GetStyle().ItemSpacing.x};
        // add hitbox
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, {0, 0, 1, 0.2});
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, {0, 0.5, 1, 0.3});
        ImGui::PushStyleColor(ImGuiCol_Header, {0, 0.5, 1, 0.3});
        ImGui::SetCursorPos(position);
        if (ImGui::Selectable("##hitbox", (selectedFile ? selectedFile->path() : "") == entry.path(),
                            ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SelectOnClick,
                            {iconSize, iconSize * yxiconRatio})) {
            selectedFile = new fs::directory_entry(entry);
            fileOffsetOnSelect = ImGui::GetWindowPos() - ImGui::GetMousePos();
            if (ImGui::IsMouseDoubleClicked(0)) {
            if (entry.is_directory()) {
                path = entry.path();
                searchbarContent = "";
            } else
                OpenInVSCode(entry.path());
            }
        }
        
        ImGui::PopStyleColor(3);
    }

    bool ShouldShwoFile(fs::directory_entry entry){
        if(entry.path().filename().string()[0]=='.')return false;
        return searchbarContent == "" || stringInString(entry.path().filename(),searchbarContent);
    }

    void DrawFile(const fs::directory_entry &entry, float iconSize, bool hasHitbox = true){
        ImGui::BeginChild(entry.path().c_str(),
                            {childsize, childsize * yxiconRatio}, 0,
                            ImGuiWindowFlags_NoScrollWithMouse |
                                ImGuiWindowFlags_NoScrollbar);
            ImGui::SameLine((ImGui::GetWindowWidth() - iconSize) / 2);
            if (entry.is_directory()) {
            rlImGuiImageSize(&folderTex, iconSize, iconSize);
            } else {
            rlImGuiImageSize(&fileTex, iconSize, iconSize);
            }
            const float textSize =
                ImGui::CalcTextSize(entry.path().filename().c_str(), nullptr,
                                    childsize)
                    .x;
            PlaceCenteredWrappedText(entry.path().filename(), childsize * 0.01);

            // Hitbox
            if(hasHitbox)
                DrawFileHitbox(entry, iconSize);
        ImGui::EndChild();
    }

    void DrawFiles(fs::directory_iterator it, float iconWinRatio = 0.40){
        ImGui::BeginChild("Files");
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float iconSize = iconWinRatio * childsize;    
        const int numIconsPerRow = (ImGui::GetWindowWidth() - 2 * spacing) / (childsize + spacing);
        int entryIndex = 0;

        for (const fs::directory_entry &entry : it) {
            if (ShouldShwoFile(entry)) {
                DrawFile(entry,iconSize);
                entryIndex++;
                if ((entryIndex % numIconsPerRow) != 0) {
                ImGui::SameLine(0);
                }
            }
        }
        ImGui::EndChild();
    }
    
    // returns on release
    fs::directory_entry* DrawSelectedFile(float iconWinRatio = 0.40){
        const float iconSize = iconWinRatio * childsize;    
        if(ImGui::IsMouseDragging(ImGuiMouseButton_Left,1)){
            ImGui::SetNextWindowPos(ImGui::GetMousePos() + fileOffsetOnSelect);
            ImGui::PushStyleColor(ImGuiCol_PopupBg,{0,0,0,0});
            ImGui::PushStyleColor(ImGuiCol_Border,{0,0,0,0});
                ImGui::BeginTooltip();
                    DrawFile(*selectedFile, iconSize, false);
                ImGui::EndTooltip();
            ImGui::PopStyleColor(2);
        }
        else{
            if(IsMouseButtonUp(ImGuiMouseButton_Left))
                selectedFile = nullptr;
        }
    }
    void DrawExplorer(){    
        ImGui::Begin("Explorer");
            ImGui::BeginChild("CopyButton",{15,15});
                rlImGuiImageSize(&copyTex,15,15);
                ImGui::SetCursorPos({0,0});
                if(ImGui::Selectable("##copy",false,0,{15,15}))
                    SetClipboardText(path.c_str());
            ImGui::EndChild();
            ImGui::SameLine();
            RenderPath();
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowWidth()/10);
            ImGui::DragFloat("##Iconsize", &childsize, .5,1,400);
            
            // Draw searchbar
            // align
            const float searchbarWidth = 200;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - searchbarWidth - ImGui::GetStyle().ItemSpacing.x);
            ImGui::BeginChild("##searchbar",{searchbarWidth,20});
                ImGui::PushItemWidth(searchbarWidth);
                ImGui::SetCursorPos({20,0});
                ImGui::InputText("##searchbar", &searchbarContent);
                ImGui::SetCursorPos({0,0});
                rlImGuiImageSize(&spyglassTex,20,20);

            ImGui::EndChild();



            std::error_code ec;
            fs::directory_iterator it(path, ec);
            if (ec) {
                // couldn't open it, show a message instead of crashing
                ImGui::TextDisabled("Can't open: %s", ec.message().c_str());
            } else {
                DrawFiles(it);
            }

        ImGui::End();
    }

private:
void PlaceCenteredWrappedText(const std::string& text, float scale, int maxLines = 2)
{
    ImGui::SetWindowFontScale(scale);

    const float pad       = 4.0f * scale;
    const float maxWidth  = ImGui::GetWindowWidth() - 2.0f * pad;
    const char* p         = text.c_str();
    const char* end       = p + text.size();

    for (int line = 0; line < maxLines && p < end; ++line)
    {
        const bool lastLine = (line == maxLines - 1);

        // Longest prefix that fits (ImGui handles UTF-8 correctly here)
        const char* lineEnd = ImGui::GetFont()->CalcWordWrapPositionA(
            scale, // scale
            p, end, maxWidth);
        if (lineEnd == p) lineEnd = p + 1; // always make progress

        std::string chunk(p, lineEnd);
        if (lastLine && lineEnd < end)
            chunk += "...";                 // truncate instead of overflowing

        const float w = ImGui::CalcTextSize(chunk.c_str()).x;
        ImGui::SetCursorPosX(std::max((ImGui::GetWindowWidth() - w) * 0.5f, 0.0f));
        ImGui::TextUnformatted(chunk.c_str());

        p = lineEnd;
    }

    ImGui::SetWindowFontScale(1.0f); // reset
}

};
