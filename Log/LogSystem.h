#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include "../Simulator/Unit.h"

#include <fstream>
#include <string>
#include <vector>

class LogSystem {
  private:
    static std::ofstream logFile;
    static bool isLogging;

  public:
    static bool StartLogging(const std::string &logFileName);

    static void StopLogging();

    static void Log(const std::string &message);

    static bool IsLogging();

    static std::string GetUnitListName(std::vector<Unit *> unitList);

    static void LogUnitListStats(std::vector<Unit *> unitList);

    template <typename... Args> static void LogStream(Args &&...args) {
        if (isLogging && logFile.is_open()) {
            (logFile << ... << args) << std::endl;
        }
    }
};

#endif