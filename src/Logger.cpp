// src/Logger.cpp
#include "Logger.h"

// Initialize the Singleton instance
Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

// Private constructor
Logger::Logger() : max_entries(1000) {}

// Log a message
void Logger::Log(LogLevel level, const std::string& message) {
    entries.emplace_back(LogEntry{ level, message });
    if (entries.size() > max_entries)
        entries.erase(entries.begin());
}

// Get all log entries
const std::vector<LogEntry>& Logger::GetEntries() const {
    return entries;
}

// Clear all log entries
void Logger::Clear() {
    entries.clear();
}
