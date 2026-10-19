// this is the new one

#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "raylib.h"
#include <algorithm>
#include <cctype>
#include <string>

#include <filesystem>
#include <vector>
#include "rlImGui.h"
#include <sstream>

#include <imgui_stdlib.h>
#include "../imguihelpers.hpp"

#include <fstream>
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
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, {0, 0, 1, 0.2});
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, {0, 0.5, 1, 0.3});
        ImGui::PushStyleColor(ImGuiCol_Header, {0, 0.5, 1, 0.3});
        ImGui::SetCursorPos(position);


        ImGuiSelectableFlags flags =
            (ImGuiSelectableFlags)ImGuiSelectableFlags_AllowDoubleClick |
            (ImGuiSelectableFlags)ImGuiSelectableFlags_SelectOnClick;

        bool selected = selectedFile && selectedFile->path() == entry.path();   
        if (ImGui::Selectable("##hitbox", selected, flags,{iconSize, iconSize * yxiconRatio})) {
            fileOffsetOnSelect = ImGui::GetWindowPos() - ImGui::GetMousePos();
            selectedFile = &entry;
            if (ImGui::IsMouseDoubleClicked(0)) {
                if (entry.is_directory()) {
                    path = entry.path();
                    searchbarContent = "";
                } else{
                    OpenInVSCode(entry.path());
                }
            }
        }

        DrawSelectedFile(entry);

        // Folders can also be drop targets (move files into them)
        if (entry.is_directory() && ImGui::BeginDragDropTarget()) {
            TraceLog(LOG_INFO, "hovering folder: %s", entry.path().c_str());
            if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload(FILE_PAYLOAD)) {
                fs::path src = (const char*)pl->Data;
                std::error_code ec;
                fs::rename(src, entry.path() / src.filename(), ec);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleColor(3);
    }

    bool ShouldShwoFile(fs::directory_entry entry){
        if(entry.path().filename().string()[0]=='.')return false;
        return searchbarContent == "" || stringInString(entry.path().filename(),searchbarContent);
    }

    void DrawFile(const fs::directory_entry &entry, float iconSize, bool hasHitbox = true){
        ImGuiWindowFlags wf = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        if (!hasHitbox) wf |= ImGuiWindowFlags_NoInputs;   // preview must not catch the mouse


        ImGui::BeginChild(entry.path().c_str(),
                {childsize, childsize * yxiconRatio}, 0,wf);
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - iconSize) / 2);
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
    fs::directory_entry* DrawSelectedFile(fs::directory_entry entry, float iconWinRatio = 0.40){
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
            ImGui::PushStyleColor(ImGuiCol_PopupBg,{0,0,0,0});
            ImGui::PushStyleColor(ImGuiCol_Border,{0,0,0,0});
            ImGui::SetNextWindowPos(ImGui::GetMousePos() + fileOffsetOnSelect);

            ImGui::BeginTooltip();
                std::string p = entry.path().string();
                ImGui::SetDragDropPayload(FILE_PAYLOAD, p.c_str(), p.size() + 1); // ImGui copies the bytes
                DrawFile(entry, childsize * 0.40f, false); // preview that follows the cursor
            ImGui::EndTooltip();
            ImGui::EndDragDropSource();
            ImGui::PopStyleColor(2);
        }
        return nullptr;
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
            ImGui::DragFloat("##Iconsize", &childsize, .5,20,400);
            
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

            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows |
                                   ImGuiHoveredFlags_AllowWhenBlockedByPopup)
                && !ImGui::IsAnyItemHovered()
                && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("explorer_popup");
            }

            if(ImGui::BeginPopup("explorer_popup", ImGuiPopupFlags_MouseButtonRight)){
                if(ImGui::MenuItem("New File")) {
                    std::ofstream file(path+"/New File");
                    printf("made new file\n");
                }
                if(ImGui::MenuItem("New Folder")) {
                    fs::create_directory(path+"/New Folder");
                    printf("made new folder\n");
                }
                ImGui::EndPopup();
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