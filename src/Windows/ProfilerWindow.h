#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include "Engine/Profiler.h" // Ensure Profiler classes are included

class ProfilerWindow
{
public:
    ProfilerWindow();
    ~ProfilerWindow() = default;

    // Render the profiler window
    void Show();

private:
    struct ProfileHistory
    {
        std::deque<double> totalTimeHistory;
        std::deque<double> averageTimeHistory;
        static const size_t MaxHistory = 100;
    };

    std::unordered_map<std::string, ProfileHistory> m_ProfileHistories;
    std::deque<double> m_TotalFrameTimeHistory;
    static const size_t MaxFrameHistory = 100;

    // Timing variables for update throttling
    double m_UpdateInterval; // Interval in seconds (0.1)
    std::chrono::steady_clock::time_point m_LastUpdateTime;

    // Helper functions
    void UpdateHistory(const std::unordered_map<std::string, ProfileResult> &data, double totalFrameTime);
    void RenderTable(const std::unordered_map<std::string, ProfileResult> &data);
    void RenderGraphs();

    // Helper for data smoothing
    std::vector<float> MovingAverage(const std::deque<double> &data, size_t window);
    std::vector<float> ExponentialMovingAverage(const std::deque<double> &data, float alpha);
};
