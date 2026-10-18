#include "imgui.h"

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x + b.x, a.y + b.y);
}

inline ImVec2 operator-(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x - b.x, a.y - b.y);
}

inline ImVec2 operator*(const ImVec2& v, float s)
{
    return ImVec2(v.x * s, v.y * s);
}

inline ImVec2 operator/(const ImVec2& v, float s)
{
    return ImVec2(v.x / s, v.y / s);
}

constexpr const char* FILE_PAYLOAD = "FILE_PATH";