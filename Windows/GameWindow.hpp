#pragma once
#include "raylib.h"
#include <vector>
#include "../Object.hpp"
static void DrawGameWindow(RenderTexture2D texture, const std::vector<Object*>& objects){
    ClearBackground(DARKGRAY);
    for(int i = 0; i< 5; i++){
        DrawRectangleV(Vector2(0,i*55), {50, 50}, RED);
        
    }
}