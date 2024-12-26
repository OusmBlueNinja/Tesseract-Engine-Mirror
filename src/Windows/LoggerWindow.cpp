// src/Windows/LoggerWindow.cpp

#include "LoggerWindow.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>

void LoggerWindow::AddLog(const char* fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_Logs.push_back(buffer);
}

void LoggerWindow::Show()
{
    ImGui::Begin("Logger");

    if (ImGui::Button("Clear"))
        m_Logs.clear();

    ImGui::Separator();
    for (const auto& logLine : m_Logs)
        ImGui::TextUnformatted(logLine.c_str());

    ImGui::End();
}
