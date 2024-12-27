// src/Windows/LoggerWindow.h

#pragma once

#include <vector>
#include <string>
#include <optional>
#include <imgui.h>

struct LogEntry {
    std::string text;
    std::optional<ImVec4> color;

    LogEntry(const std::string& msg, std::optional<ImVec4> col = std::nullopt)
        : text(msg), color(col) {}
};

class LoggerWindow {
public:
    void AddLog(const char* fmt, ...);
    void AddLog(const char* fmt, std::optional<ImVec4> color, ...);
    void Show();

private:
    std::vector<LogEntry> m_Logs;
    bool m_ScrollToBottom = false;
};
