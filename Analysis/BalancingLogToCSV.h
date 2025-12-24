#ifndef BALANCINGLOG_TO_CSV_H
#define BALANCINGLOG_TO_CSV_H

#include "../AutoBalancing/BalancingLog.h"
#include <string>

class BalancingLogToCSV {
  public:
    static void Convert();
    static void SetExperimentPath(const std::string &path);
    static std::string GetExperimentPath();

  private:
    static std::string experimentPath;
};

#endif