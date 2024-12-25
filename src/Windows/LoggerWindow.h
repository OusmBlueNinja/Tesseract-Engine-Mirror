// src/Windows/LoggerWindow.h

#pragma once
#include <vector>
#include <string>

class LoggerWindow
{
public:
    // Add logs from anywhere
    void AddLog(const char* fmt, ...);

    // Show the ImGui window
    void Show();

private:
    std::vector<std::string> m_Logs;
};
