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

    void UpdateHistory(const std::unordered_map<std::string, ProfileResult> &data, double totalFrameTime);
    void Show();
    void RenderTable();
    void RenderGraphs();

private:
    struct ProfileHistory
    {
        static const size_t MaxHistory = 100;
        std::deque<double> totalTimeHistory;   // Last N total times
        std::deque<double> averageTimeHistory; // Last N average times
        std::deque<int> callCountHistory;      // Last N call counts
    };

    std::unordered_map<std::string, ProfileHistory> m_ProfileHistories;
    std::deque<double> m_TotalFrameTimeHistory;
    static const size_t MaxFrameHistory = 100;

    std::chrono::steady_clock::time_point m_LastUpdateTime;
    double m_UpdateInterval; // In seconds

    // Helper function for Exponential Moving Average
    std::vector<float> ExponentialMovingAverage(const std::deque<double> &data, float alpha);
};