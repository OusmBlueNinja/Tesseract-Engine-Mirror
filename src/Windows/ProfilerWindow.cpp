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




std::vector<float> ProfilerWindow::ExponentialMovingAverage(const std::deque<double>& data, float alpha) {
    std::vector<float> ema;
    ema.reserve(data.size());
    float prev = 0.0f;
    for (const auto& val : data) {
        prev = alpha * static_cast<float>(val) + (1.0f - alpha) * prev;
        ema.push_back(prev);
    }
    return ema;
}

void ProfilerWindow::UpdateHistory(const std::unordered_map<std::string, ProfileResult>& data, double totalFrameTime)
{
    // Update total frame time history
    m_TotalFrameTimeHistory.push_back(totalFrameTime);
    if (m_TotalFrameTimeHistory.size() > MaxFrameHistory)
        m_TotalFrameTimeHistory.pop_front();

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

        // Update call count history
        history.callCountHistory.push_back(result.CallCount);
        if (history.callCountHistory.size() > ProfileHistory::MaxHistory)
            history.callCountHistory.pop_front();
    }

    // Ensure that functions not present in the current frame retain their last TotalTime and AverageTime
    for (auto& [name, history] : m_ProfileHistories)
    {
        if (data.find(name) == data.end())
        {
            // Retain last TotalTime and AverageTime by pushing back the last value again
            if (!history.totalTimeHistory.empty())
                history.totalTimeHistory.push_back(history.totalTimeHistory.back());
            else
                history.totalTimeHistory.push_back(0.0);

            if (!history.averageTimeHistory.empty())
                history.averageTimeHistory.push_back(history.averageTimeHistory.back());
            else
                history.averageTimeHistory.push_back(0.0);

            // Update call count history with zero for this frame
            history.callCountHistory.push_back(0);

            // Maintain history sizes
            if (history.totalTimeHistory.size() > ProfileHistory::MaxHistory)
                history.totalTimeHistory.pop_front();
            if (history.averageTimeHistory.size() > ProfileHistory::MaxHistory)
                history.averageTimeHistory.pop_front();
            if (history.callCountHistory.size() > ProfileHistory::MaxHistory)
                history.callCountHistory.pop_front();
        }
    }
}

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

    // Begin ImGui window with improved styling
    ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);

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
    RenderTable();

    // Render profiling graphs
    RenderGraphs();

    // Display total frame time (from the last update)
    if (!m_TotalFrameTimeHistory.empty())
    {
        double lastTotalFrameTime = m_TotalFrameTimeHistory.back();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 0.0f, 1.0f), "Total Frame Time: %.3f µs", lastTotalFrameTime);
    }

    ImGui::End();
}

void ProfilerWindow::RenderTable()
{
    // Collect all profiling histories
    std::vector<std::pair<std::string, ProfileHistory>> allData(m_ProfileHistories.begin(), m_ProfileHistories.end());

    // Sort functions by last Total Time descending
    std::sort(allData.begin(), allData.end(),
        [](const std::pair<std::string, ProfileHistory>& a, const std::pair<std::string, ProfileHistory>& b) -> bool {
            return a.second.totalTimeHistory.back() > b.second.totalTimeHistory.back();
        });

    // Add a filter input with enhanced styling
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
    static char filterBuffer[128] = "";
    ImGui::InputTextWithHint("##Filter", "Filter functions...", filterBuffer, IM_ARRAYSIZE(filterBuffer));
    ImGui::PopStyleVar();

    // Convert filter to string
    std::string filterStr = filterBuffer;

    // Filtered data
    std::vector<std::pair<std::string, ProfileHistory>> filteredData;
    for (const auto& [name, history] : allData)
    {
        if (filterStr.empty() || name.find(filterStr) != std::string::npos)
            filteredData.emplace_back(name, history);
    }

    // Define threshold for highlighting (e.g., 1000 µs)
    const double highlightThreshold = 1000.0;

    // Improved table with sorting indicators and better aesthetics
    if (ImGui::BeginTable("ProfilerTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, 300)))
    {
        // Set up columns with sortable headers
        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Total Time (µs)", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Average Time (µs)", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Calls (This Frame)", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        // Alternate row colors for better readability
        bool rowBg = false;

        for (const auto& [name, history] : filteredData)
        {
            ImGui::TableNextRow();
            rowBg = !rowBg;
            if (rowBg)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(0.1f, 0.1f, 0.1f, 1.0f));

            // Function Name with tooltip
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(name.c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Function: %s", name.c_str());
                ImGui::Text("Total Time: %.3f µs", history.totalTimeHistory.back());
                ImGui::Text("Average Time: %.3f µs", history.averageTimeHistory.back());
                ImGui::Text("Call Count (this frame): %d", history.callCountHistory.back());
                ImGui::EndTooltip();
            }

            // Total Time with color coding
            ImGui::TableSetColumnIndex(1);
            if (history.totalTimeHistory.back() > highlightThreshold)
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%.3f", history.totalTimeHistory.back());
            else
                ImGui::Text("%.3f", history.totalTimeHistory.back());

            // Average Time
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.3f", history.averageTimeHistory.back());

            // Call Count (This Frame)
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", history.callCountHistory.back());
        }

        ImGui::EndTable();
    }
}

void ProfilerWindow::RenderGraphs()
{
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.0f, 0.7f, 0.9f, 1.0f), "Profiling Graphs (Top 5 Functions)");

    // Collect and sort functions by last Total Time descending
    std::vector<std::pair<std::string, ProfileHistory>> sortedData(m_ProfileHistories.begin(), m_ProfileHistories.end());
    std::sort(sortedData.begin(), sortedData.end(),
        [](const std::pair<std::string, ProfileHistory>& a, const std::pair<std::string, ProfileHistory>& b) -> bool {
            return a.second.totalTimeHistory.back() > b.second.totalTimeHistory.back();
        });

    size_t displayCount = std::min<size_t>(5, sortedData.size()); // Limit to top 5 functions

    // Prepare data for the unified plot with EMA smoothing
    std::vector<std::vector<float>> plotData(displayCount);
    std::vector<std::string> functionNames;

    float alpha = 0.2f; // Smoothing factor for EMA
    for (size_t i = 0; i < displayCount; ++i)
    {
        const auto& [name, history] = sortedData[i];
        functionNames.push_back(name);

        // Smooth each function's data using EMA
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
        ImVec2 graphSize = ImVec2(0, 200); // Graph dimensions

        // Begin child region for better layout control
        ImGui::BeginChild("GraphChild", graphSize, false, ImGuiWindowFlags_NoScrollbar);

        // Plot each function's history as separate lines with unique colors
        for (size_t i = 0; i < displayCount; ++i)
        {
            ImU32 color = ImColor::HSV(static_cast<float>(i) / displayCount, 0.6f, 0.9f);
            ImGui::PushStyleColor(ImGuiCol_PlotLines, color);
            ImGui::PlotLines(
                functionNames[i].c_str(),
                plotData[i].data(),
                static_cast<int>(plotData[i].size()),
                0,
                nullptr,
                0.0f,
                static_cast<float>(maxValue) * 1.1f, // Add some padding to the max value
                ImVec2(0, 100)
            );
            ImGui::PopStyleColor();
        }

        ImGui::EndChild();

        // Add a legend for the lines
        ImGui::Separator();
        for (size_t i = 0; i < functionNames.size(); ++i)
        {
            ImVec4 lineColor = ImColor::HSV(static_cast<float>(i) / displayCount, 0.6f, 0.9f);
            ImGui::SameLine();
            ImGui::ColorButton(("##Color" + std::to_string(i)).c_str(), lineColor, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoBorder, ImVec2(10, 10));
            ImGui::SameLine();
            ImGui::TextUnformatted(functionNames[i].c_str());
        }
    }
}