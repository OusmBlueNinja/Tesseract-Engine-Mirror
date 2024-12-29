// ScopedTimer.h
#pragma once

#include <string>
#include <chrono>

class Profiler; // Forward declaration

class ScopedTimer {
public:
    ScopedTimer(const std::string& name);
    ~ScopedTimer();

private:
    std::string m_Name;
    std::chrono::high_resolution_clock::time_point m_StartTime;
};
