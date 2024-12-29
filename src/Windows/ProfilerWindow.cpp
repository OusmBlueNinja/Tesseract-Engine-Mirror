#include "ProfilerWindow.h"
#include <imgui.h>
#include <algorithm>
#include <string>
#include <iostream> // For debug statements

// Constructor
ProfilerWindow::ProfilerWindow()
    : m_UpdateInterval(0.1) // Set update interval to 0.1 seconds
{
    // Initialize m_LastUpdateTime to force an immediate update on the first frame
    m_LastUpdateTime = std::chrono::steady_clock::now() - std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(m_UpdateInterval));

}

// Calculate moving average
std::vector<float> ProfilerWindow::MovingAverage(const std::deque<double>& data, size_t window)
{
    std::vector<float> averages;
    if (data.size() < window)
        window = data.size();

    for (size_t i = 0; i <= data.size() - window; ++i)
    {
        double sum = 0.0;
        for (size_t j = i; j < i + window; ++j)
            sum += data[j];
        averages.push_back(static_cast<float>(sum / window));
    }
    return averages;
}

// Update the history data structures with the latest profiling data
void ProfilerWindow::UpdateHistory(const std::unordered_map<std::string, ProfileResult>& data, double totalFrameTime)
{
    // Update total frame time history
    m_TotalFrameTimeHistory.push_back(totalFrameTime);
    if (m_TotalFrameTimeHistory.size() > MaxFrameHistory)
        m_TotalFrameTimeHistory.pop_front();

    // Debug: Print the size of m_TotalFrameTimeHistory

    // Update each function's profiling history
    for (const auto& [name, result] : data)
    {
        auto& history = m_ProfileHistories[name];

        // Update total time history
        history.totalTimeHistory.push_back(result.TotalTime);
        if (history.totalTimeHistory.size() > ProfileHistory::MaxHistory)
            history.totalTimeHistory.pop_front();

        // Update average time history
        double average = result.CallCount > 0 ? result.TotalTime / result.CallCount : 0.0;
        history.averageTimeHistory.push_back(average);
        if (history.averageTimeHistory.size() > ProfileHistory::MaxHistory)
            history.averageTimeHistory.pop_front();
    }
}

// Render the profiler window with table and graphs
void ProfilerWindow::Show()
{
    // Check if it's time to update the profiler data
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - m_LastUpdateTime;

    bool shouldUpdate = false;
    if (elapsed.count() >= m_UpdateInterval)
    {
        shouldUpdate = true;
        m_LastUpdateTime = now;
    }

    // Begin ImGui window
    ImGui::Begin("Profiler");

    const auto& data = Profiler::Get().GetLastFrameData();

    if (data.empty())
    {
        ImGui::Text("No profiling data available.");
        ImGui::End();
        return;
    }

    if (shouldUpdate)
    {
        // Calculate total frame time
        double totalFrameTime = 0.0;
        for (const auto& [name, result] : data)
        {
            totalFrameTime += result.TotalTime;
        }

        // Update history data
        UpdateHistory(data, totalFrameTime);

        // Reset profiling data for the next interval
        Profiler::Get().EndFrame();
    }

    // Render profiling data table
    RenderTable(data);

    // Render profiling graphs
    RenderGraphs();

    // Display total frame time (from the last update)
    if (!m_TotalFrameTimeHistory.empty())
    {
        double lastTotalFrameTime = m_TotalFrameTimeHistory.back();
        ImGui::Separator();
        ImGui::Text("Total Frame Time: %.3f µs", lastTotalFrameTime);
    }

    ImGui::End();
}

// Render the profiling data table
void ProfilerWindow::RenderTable(const std::unordered_map<std::string, ProfileResult>& data)
{
    // Sort functions by total time descending
    std::vector<std::pair<std::string, ProfileResult>> sortedData(data.begin(), data.end());
    std::sort(sortedData.begin(), sortedData.end(),
        [](const std::pair<std::string, ProfileResult>& a, const std::pair<std::string, ProfileResult>& b) -> bool {
            return a.second.TotalTime > b.second.TotalTime;
        });

    // Add a filter input
    static char filterBuffer[128] = "";
    ImGui::InputText("Filter", filterBuffer, IM_ARRAYSIZE(filterBuffer));

    // Convert filter to string
    std::string filterStr = filterBuffer;

    // Filtered data
    std::vector<std::pair<std::string, ProfileResult>> filteredData;
    for (const auto& [name, result] : sortedData)
    {
        if (filterStr.empty() || name.find(filterStr) != std::string::npos)
            filteredData.emplace_back(name, result);
    }

    // Define threshold for highlighting (e.g., 1000 µs)
    const double highlightThreshold = 1000.0;

    // Table with sorted data
    if (ImGui::BeginTable("ProfilerTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Total Time (µs)", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Average Time (µs)", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Calls", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        for (const auto& [name, result] : filteredData)
        {
            ImGui::TableNextRow();

            // Function Name with tooltip
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(name.c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Total Time: %.3f µs", result.TotalTime);
                double average = result.CallCount > 0 ? result.TotalTime / result.CallCount : 0.0;
                ImGui::Text("Average Time: %.3f µs", average);
                ImGui::Text("Call Count: %d", result.CallCount);
                ImGui::EndTooltip();
            }

            // Total Time with color coding
            ImGui::TableSetColumnIndex(1);
            if (result.TotalTime > highlightThreshold)
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%.3f", result.TotalTime);
            else
                ImGui::Text("%.3f", result.TotalTime);

            // Average Time
            ImGui::TableSetColumnIndex(2);
            double average = result.CallCount > 0 ? result.TotalTime / result.CallCount : 0.0;
            ImGui::Text("%.3f", average);

            // Call Count
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", result.CallCount);
        }

        ImGui::EndTable();
    }
}

// Render the profiling graphs
void ProfilerWindow::RenderGraphs()
{
    ImGui::Separator();
    ImGui::Text("Profiling Graphs");

    // Example: Render a bar graph for the top 5 functions by total time
    std::vector<std::pair<std::string, ProfileResult>> sortedData;

    const auto& data = Profiler::Get().GetLastFrameData();
    for (const auto& [name, result] : data)
    {
        sortedData.emplace_back(name, result);
    }

    // Sort and take top 5
    std::sort(sortedData.begin(), sortedData.end(),
        [](const std::pair<std::string, ProfileResult>& a, const std::pair<std::string, ProfileResult>& b) -> bool {
            return a.second.TotalTime > b.second.TotalTime;
        });

    size_t displayCount = std::min<size_t>(5, sortedData.size());

    for (size_t i = 0; i < displayCount; ++i)
    {
        const auto& [name, result] = sortedData[i];
        double percentage = 0.0;

        if (!m_TotalFrameTimeHistory.empty())
        {
            // Prevent division by zero
            double lastTotalFrameTime = m_TotalFrameTimeHistory.back();
            if (lastTotalFrameTime > 0.0)
            {
                percentage = (result.TotalTime / lastTotalFrameTime) * 100.0;
            }
        }

        ImGui::PushID(static_cast<int>(i));
        ImGui::Text("%s", name.c_str());
        ImGui::SameLine();
        ImGui::ProgressBar(static_cast<float>(percentage / 100.0f), ImVec2(-1.0f, 0.0f),
                           (std::to_string(percentage) + "%").c_str());
        ImGui::PopID();
    }

    // Example: Render a line plot for total frame time with moving average
    if (!m_TotalFrameTimeHistory.empty())
    {
        ImGui::Text("Frame Time Over Last %zu Frames (Smoothed)", m_TotalFrameTimeHistory.size());

        // Calculate moving average with a window of 10 frames
        size_t windowSize = 10;
        std::vector<float> smoothedFrameTimes = MovingAverage(m_TotalFrameTimeHistory, windowSize);

        ImGui::PlotLines("##FrameTimeSmoothed", smoothedFrameTimes.data(), static_cast<int>(smoothedFrameTimes.size()), 0, NULL, 0.0f, 1000.0f, ImVec2(0, 80));
    }
}
