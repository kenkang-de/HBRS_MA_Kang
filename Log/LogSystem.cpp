#include "LogSystem.h"
#include <iostream>

// Define static members
std::ofstream LogSystem::logFile;
bool LogSystem::isLogging = false;

bool LogSystem::StartLogging(const std::string& logFileName) {
    if (isLogging) {
        StopLogging(); // Close previous log if open
    }
    
    logFile.open(logFileName);
    if (logFile.is_open()) {
        isLogging = true;
        return true;
    }
    
    std::cerr << "Failed to open log file: " << logFileName << std::endl;
    return false;
}

void LogSystem::StopLogging() {
    if (isLogging && logFile.is_open()) {
        logFile.close();
    }
    isLogging = false;
}

void LogSystem::Log(const std::string& message) {
    if (isLogging && logFile.is_open()) {
        logFile << message << std::endl;
    }
}

