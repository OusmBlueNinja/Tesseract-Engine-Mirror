#include "PerformanceWindow.h"
#include "imgui.h"
#include <algorithm> // for std::max_element, etc.
#include "Engine/ThemeManager.h"




extern int LoaddedAssets;
extern int g_GPU_Triangles_drawn_to_screen;

// Initialize static members
int PerformanceWindow::m_OpenGLCallCount = 0;
int PerformanceWindow::m_TriangleCount = 0;

// We'll store up to 60 data points for each stat.
static float s_FpsHistory[60] = {0.0f};
static float s_MsHistory[60] = {0.0f};
static float s_CallsHistory[60] = {0.0f};
static float s_TriangleHistory[60] = {0.0f};

// Current dynamic max scale for FPS and ms
static float s_FpsScale = 120.0f; // default starting scale for FPS
static float s_MsScale = 25.0f;   // default starting scale for ms

// This function shifts the old values left and appends a new value at the end.
static void PushValueToHistory(float *historyArray, int historySize, float newValue)
{
    for (int i = 0; i < historySize - 1; i++)
        historyArray[i] = historyArray[i + 1];
    historyArray[historySize - 1] = newValue;
}

// We'll track when we last pushed data to our history.
static double s_LastPushTime = 0.0;

// We'll also track when we last updated the scale
static double s_LastScaleUpdate = 0.0;

// Update counters from the outside
void PerformanceWindow::UpdatePerformanceStats(int newCallCount, int newTriangleCount)
{
    m_OpenGLCallCount = newCallCount;
    m_TriangleCount = newTriangleCount;

    g_GPU_Triangles_drawn_to_screen = 0;

}

void PerformanceWindow::Show(float fps, float ms)
{
    // 1) Get current time from ImGui's internal clock
    double currentTime = ImGui::GetTime();

    // 2) If at least 0.05s has passed, push new data (about 20 updates per second)
    if ((currentTime - s_LastPushTime) >= 0.05)
    {
        s_LastPushTime = currentTime;

        // Push new values into our history arrays
        PushValueToHistory(s_FpsHistory, 60, fps);
        PushValueToHistory(s_MsHistory, 60, ms);
        PushValueToHistory(s_CallsHistory, 60, (float)m_OpenGLCallCount);
        PushValueToHistory(s_TriangleHistory, 60, (float)m_TriangleCount);
    }

    // 3) Every 1 second, recalculate the max scale for FPS and ms
    if ((currentTime - s_LastScaleUpdate) >= 1.0)
    {
        s_LastScaleUpdate = currentTime;

        // Find the maximum in s_FpsHistory
        float maxFps = 0.0f;
        for (int i = 0; i < 60; i++)
        {
            if (s_FpsHistory[i] > maxFps)
                maxFps = s_FpsHistory[i];
        }
        // Scale it by +15%, ensure it's not below 1.0
        maxFps *= 1.15f;
        if (maxFps < 1.0f)
            maxFps = 1.0f;
        s_FpsScale = maxFps;

        // Find the maximum in s_MsHistory
        float maxMs = 0.0f;
        for (int i = 0; i < 60; i++)
        {
            if (s_MsHistory[i] > maxMs)
                maxMs = s_MsHistory[i];
        }
        // Scale it by +15%, ensure it's not below 1.0
        maxMs *= 1.15f;
        if (maxMs < 1.0f)
            maxMs = 1.0f;
        s_MsScale = maxMs;
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
    // min = 0, max = s_FpsScale or s_MsScale
    ImGui::PlotLines("FPS",
                     s_FpsHistory,
                     IM_ARRAYSIZE(s_FpsHistory),
                     0,
                     nullptr,
                     0.0f,
                     s_FpsScale,
                     ImVec2(0, 60));

    ImGui::PlotHistogram("ms/frame",
                         s_MsHistory,
                         IM_ARRAYSIZE(s_MsHistory),
                         0,
                         nullptr,
                         0.0f,
                         s_MsScale,
                         ImVec2(0, 60));

    ImGui::Separator();

    // Show OpenGL calls + Triangles
    ImGui::Text("OpenGL Calls: %d", m_OpenGLCallCount);
    ImGui::PlotLines("GL Calls",
                     s_CallsHistory,
                     IM_ARRAYSIZE(s_CallsHistory),
                     0,
                     nullptr,
                     0.0f,
                     300.0f,
                     ImVec2(0, 50));

    ImGui::Text("Indices: %d", m_TriangleCount);
    ImGui::PlotHistogram("Indices",
                         s_TriangleHistory,
                         IM_ARRAYSIZE(s_TriangleHistory),
                         0,
                         nullptr,
                         0.0f,
                         m_TriangleCount*2.5,
                         ImVec2(0, 50));

    ImGui::Separator();

    // Show asset count
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Assets: %d", LoaddedAssets);

    ImGui::Separator();



    const char *options[] = {"Bootsrap", "Duck Red", "Windark"};
    static int current_option = -1; // No selection initially

    const char *preview_value = (current_option >= 0 && current_option < 3) ? options[current_option] : "Select an option";
    if (ImGui::BeginCombo("Theme", preview_value))
    {
        for (int n = 0; n < IM_ARRAYSIZE(options); n++)
        {
            bool is_selected = (current_option == n);
            if (ImGui::Selectable(options[n], is_selected))
            {
                current_option = n;       // Update current option
                ThemeManager_ChangeTheme(n); // Call the function with the selected option
            }
            // Set the initial focus when opening the combo (optional)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
