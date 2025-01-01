// src/Windows/LoggerWindow.cpp

#include "LoggerWindow.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>
#include "Icons.h"

// Helper function to format strings
static std::string FormatString(const char* fmt, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    return std::string(buffer);
}

void LoggerWindow::AddLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string formatted = FormatString(fmt, args);
    va_end(args);

    m_Logs.emplace_back(formatted);
    m_ScrollToBottom = true;
}

void LoggerWindow::AddLog(const char* fmt, std::optional<ImVec4> color, ...) {
    va_list args;
    va_start(args, color);
    std::string formatted = FormatString(fmt, args);
    va_end(args);

    m_Logs.emplace_back(formatted, color);
    m_ScrollToBottom = true;
}

void LoggerWindow::Show() {
    ImGui::Begin(ICON_FA_TERMINAL " Logger##logger");

    if (ImGui::Button("Clear")) {
        m_Logs.clear();
    }

    ImGui::Separator();

    // Begin a child region to enable scrolling
    ImGui::BeginChild("LoggerScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& logEntry : m_Logs) {
        if (logEntry.color.has_value()) {
            ImGui::PushStyleColor(ImGuiCol_Text, logEntry.color.value());
            ImGui::TextUnformatted(logEntry.text.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextUnformatted(logEntry.text.c_str());
        }
    }

    // Auto-scroll to bottom if new logs are added
    if (m_ScrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        m_ScrollToBottom = false;
    }

    ImGui::EndChild();

    ImGui::End();
}
