// src/Windows/PerformanceWindow.h

#pragma once

class PerformanceWindow
{
public:
    // Displays the performance window in ImGui.
    // fps  = frames per second
    // ms   = milliseconds per frame
    void Show(float fps, float ms);

    // Updates the counters for OpenGL calls and triangle count.
    void UpdatePerformanceStats(int newCallCount, int newTriangleCount);

private:
    // These are static so they're shared across all instances
    static int m_OpenGLCallCount;
    static int m_TriangleCount;
};
