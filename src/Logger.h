// src/Logger.h
#pragma once

#include <vector>
#include <string>
#include <cstdint>

// Enum for log levels
enum class LogLevel { INFO, WARNING, ERROR };

// Struct to represent a log entry
struct LogEntry {
    LogLevel level;
    std::string message;
};

// Logger class (Singleton)
class Logger {
public:
    static Logger& GetInstance();

    void Log(LogLevel level, const std::string& message);
    const std::vector<LogEntry>& GetEntries() const;
    void Clear();

private:
    Logger(); // Private constructor for Singleton
    std::vector<LogEntry> entries;
    size_t max_entries;
};
