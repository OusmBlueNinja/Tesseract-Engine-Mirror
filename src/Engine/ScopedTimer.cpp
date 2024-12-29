// ScopedTimer.cpp
#include "ScopedTimer.h"
#include "Profiler.h"

ScopedTimer::ScopedTimer(const std::string& name)
    : m_Name(name), m_StartTime(std::chrono::high_resolution_clock::now()) {}

ScopedTimer::~ScopedTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::micro>(endTime - m_StartTime).count(); // Duration in microseconds
    Profiler::Get().AddProfileResult(m_Name, duration);
}
