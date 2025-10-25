#ifndef BALANCINGLOG_H
#define BALANCINGLOG_H

#include <string>
#include <vector>

#include "../Simulator/Constants.h"

class BalancingLog {
  public:
    static std::vector<std::vector<std::string>> BalancingLogs;

    static void InitializeLogs(int maxGenerations) {
        BalancingLogs.clear();
        BalancingLogs.resize(maxGenerations);
    }

    BalancingLog(int generation, float fitness, float mRSE, float dOC) {
        std::string log = std::to_string(fitness) + " (" + std::to_string(mRSE) + " + " + std::to_string(dOC) + ")";
        BalancingLogs[generation - 1].push_back(log);
    }
};

#endif