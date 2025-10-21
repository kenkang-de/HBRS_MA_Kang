#ifndef BALANCINGLOG_H
#define BALANCINGLOG_H

#include <string>
#include <vector> 

#include "../Simulator/Constants.h"

class BalancingLog
{
public: 

static std::vector<std::vector<std::string>> BalancingLogs;

static void InitializeLogs(int maxGenerations) {
    BalancingLogs.clear();
    BalancingLogs.resize(maxGenerations);
}

BalancingLog(int generation, float fitness, float mRSE, float dOG) 
{
    std::string log = std::to_string(fitness) + " (" + std::to_string(mRSE) + " + " + std::to_string(dOG) + ")";
    BalancingLogs[generation-1].push_back(log);
}

};

#endif