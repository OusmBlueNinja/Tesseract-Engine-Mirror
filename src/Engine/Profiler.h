// Profiler.h
#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

struct ProfileResult {
    double TotalTime; // Total time in microseconds
    int CallCount;
};

class Profiler {
public:
    static Profiler& Get() {
        static Profiler instance;
        return instance;
    }

    void AddProfileResult(const std::string& name, double time) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto& result = m_ProfileData[name];
        result.TotalTime += time;
        result.CallCount += 1;
    }

    // Call this at the end of each frame to prepare data for display
    void EndFrame() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_LastFrameData = m_ProfileData;
        m_ProfileData.clear();
    }

    const std::unordered_map<std::string, ProfileResult>& GetLastFrameData() const {
        return m_LastFrameData;
    }

private:
    Profiler() {}
    ~Profiler() {}
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    std::unordered_map<std::string, ProfileResult> m_ProfileData;
    std::unordered_map<std::string, ProfileResult> m_LastFrameData;
    mutable std::mutex m_Mutex;
};
