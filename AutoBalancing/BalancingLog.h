#ifndef BALANCINGLOG_H
#define BALANCINGLOG_H

#include <string>
#include <vector> 

#include "../Simulator/Constants.h"

class BalancingLog
{
public: 

static std::vector<std::vector<std::string>> BalancingLogs;

BalancingLog(int generation, float mRSE, float dOG, float fitness) 
{
    std::string log = std::to_string(fitness) + " (" + std::to_string(mRSE) + " + " + std::to_string(dOG) + ")";
    BalancingLogs[generation-1].push_back(log);
}

};

#endif