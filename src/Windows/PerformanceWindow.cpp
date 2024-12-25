// src/Windows/PerformanceWindow.cpp

#include "PerformanceWindow.h"
#include "imgui.h"

void PerformanceWindow::Show(float fps, float ms)
{
    ImGui::Begin("Performance");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("ms/frame: %.3f ms", ms);
    ImGui::Separator();
    ImGui::Text("You can display more stats here (CPU/GPU usage, etc.)");
    ImGui::End();
}
