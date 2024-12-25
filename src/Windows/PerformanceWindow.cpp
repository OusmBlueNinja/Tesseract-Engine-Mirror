#include "PerformanceWindow.h"
#include "imgui.h"
#include <algorithm> // for std::move, etc.

extern int LoaddedAssets;

// Initialize static members
int PerformanceWindow::m_OpenGLCallCount = 0;
int PerformanceWindow::m_TriangleCount   = 0;

// We'll store up to 60 data points for each stat.
static float s_FpsHistory[60]      = {0.0f};
static float s_MsHistory[60]       = {0.0f};
static float s_CallsHistory[60]    = {0.0f};
static float s_TriangleHistory[60] = {0.0f};

// This function shifts the old values left and appends a new value at the end.
static void PushValueToHistory(float* historyArray, int historySize, float newValue)
{
    for (int i = 0; i < historySize - 1; i++)
        historyArray[i] = historyArray[i + 1];
    historyArray[historySize - 1] = newValue;
}

// We'll track when we last pushed data to our history.
static double s_LastPushTime = 0.0;

// Update counters from the outside
void PerformanceWindow::UpdatePerformanceStats(int newCallCount, int newTriangleCount)
{
    m_OpenGLCallCount = newCallCount;
    m_TriangleCount   = newTriangleCount;
}

void PerformanceWindow::Show(float fps, float ms)
{
    // Get current time from ImGui's internal clock
    double currentTime = ImGui::GetTime(); 
    // If at least 1ms (0.001s) has passed, push new data
    if ((currentTime - s_LastPushTime) >= 0.05)
    {
        s_LastPushTime = currentTime;

        // Push new values into our history arrays
        PushValueToHistory(s_FpsHistory,      60, fps);
        PushValueToHistory(s_MsHistory,       60, ms);
        PushValueToHistory(s_CallsHistory,    60, (float)m_OpenGLCallCount);
        PushValueToHistory(s_TriangleHistory, 60, (float)m_TriangleCount);
    }

    // Optional style adjustments
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::Begin("Performance");

    // Colored header
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Performance Stats");
    ImGui::Separator();

    // Show current FPS/ms
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "FPS: %.1f", fps);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.8f, 1.0f), "| ms: %.3f", ms);

    // Graphs for FPS + MS
    ImGui::PlotLines("FPS (60 frames)", s_FpsHistory, IM_ARRAYSIZE(s_FpsHistory),
                     0, nullptr, 0.0f, 1000.0f, ImVec2(0, 60));
    ImGui::PlotHistogram("ms/frame", s_MsHistory, IM_ARRAYSIZE(s_MsHistory),
                         0, nullptr, 0.0f, 25.0f, ImVec2(0, 60));

    ImGui::Separator();

    // Show OpenGL calls + Triangles
    ImGui::Text("OpenGL Calls: %d", m_OpenGLCallCount);
    ImGui::PlotLines("GL Calls (60 frames)", s_CallsHistory, IM_ARRAYSIZE(s_CallsHistory),
                     0, nullptr, 0.0f, 300.0f, ImVec2(0, 50));

    ImGui::Text("Triangles: %d", m_TriangleCount);
    ImGui::PlotHistogram("Triangles (60 frames)", s_TriangleHistory, IM_ARRAYSIZE(s_TriangleHistory),
                         0, nullptr, 0.0f, 5000.0f, ImVec2(0, 50));

    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Assets: %d", LoaddedAssets);

    ImGui::End();
    ImGui::PopStyleVar();
}
