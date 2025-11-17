#ifndef BALANCINGLOG_TO_CSV_H
#define BALANCINGLOG_TO_CSV_H

#include "../AutoBalancing/BalancingLog.h"
#include <string>

class BalancingLogToCSV {
  public:
    static void Convert();
    static void InitializeRunDirectory();
    static void SetExperimentNumber(int experimentNumber);
    static std::string GetSharedDirectory();

  private:
    static std::string sharedRunDirectory;
    static int currentExperimentNumber;
};

#endif