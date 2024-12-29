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

std::vector<float> ProfilerWindow::ExponentialMovingAverage(const std::deque<double>& data, float alpha)
{
    std::vector<float> smoothedData;
    if (data.empty())
        return smoothedData;

    float ema = static_cast<float>(data[0]); // Initialize EMA with the first value
    smoothedData.push_back(ema);

    for (size_t i = 1; i < data.size(); ++i)
    {
        ema = alpha * static_cast<float>(data[i]) + (1 - alpha) * ema;
        smoothedData.push_back(ema);
    }

    return smoothedData;
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
    //RenderGraphs();

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




void ProfilerWindow::RenderGraphs()
{
    ImGui::Separator();
    ImGui::Text("Profiling Graphs (Unified Multi-Line Plot)");

    const auto& data = Profiler::Get().GetLastFrameData();
    std::vector<std::pair<std::string, ProfileResult>> sortedData;

    // Collect and sort functions by total time used
    for (const auto& [name, result] : data)
    {
        sortedData.emplace_back(name, result);
    }

    std::sort(sortedData.begin(), sortedData.end(),
              [](const std::pair<std::string, ProfileResult>& a, const std::pair<std::string, ProfileResult>& b) -> bool {
                  return a.second.TotalTime > b.second.TotalTime;
              });

    size_t displayCount = std::min<size_t>(5, sortedData.size()); // Limit to top 5 functions

    // Prepare data for the unified plot
    std::vector<std::vector<float>> plotData(displayCount);
    std::vector<std::string> functionNames;

    float alpha = 0.2f; // Smoothing factor for EMA
    for (size_t i = 0; i < displayCount; ++i)
    {
        const auto& [name, result] = sortedData[i];
        functionNames.push_back(name);

        // Smooth each function's data using EMA
        const auto& history = m_ProfileHistories[name];
        plotData[i] = ExponentialMovingAverage(history.totalTimeHistory, alpha);
    }

    // Find the longest data series and the maximum value for normalization
    size_t maxHistorySize = 0;
    float maxValue = 0.0f;
    for (const auto& series : plotData)
    {
        if (!series.empty())
        {
            maxHistorySize = std::max(maxHistorySize, series.size());
            maxValue = std::max(maxValue, *std::max_element(series.begin(), series.end()));
        }
    }

    // Prepare the combined graph
    if (maxHistorySize > 0)
    {
        std::vector<float> combinedGraph(maxHistorySize, 0.0f);

        // Render a single unified graph with multi-line data
        ImVec2 graphSize = ImVec2(0, 200); // Graph dimensions

        ImGui::PlotLines(
            "##UnifiedGraph",
            [](void* data, int idx) -> float {
                auto* plotData = static_cast<std::vector<std::vector<float>>*>(data);
                float value = 0.0f;

                for (const auto& series : *plotData)
                {
                    if (idx < series.size())
                        value += series[idx];
                }

                return value;
            },
            static_cast<void*>(&plotData), static_cast<int>(maxHistorySize), 0, nullptr, 0.0f, maxValue, graphSize);

        // Add a legend for the lines
        ImGui::Separator();
        for (size_t i = 0; i < functionNames.size(); ++i)
        {
            ImVec4 lineColor = ImVec4(0.2f + 0.2f * i, 0.2f, 1.0f - 0.2f * i, 1.0f);
            ImGui::TextColored(lineColor, "%s", functionNames[i].c_str());
        }
    }
}
