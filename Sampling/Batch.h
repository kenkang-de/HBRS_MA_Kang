#ifndef BATCH_H
#define BATCH_H

#include <vector>
#include <string>

#include "SimulationTeamSetting.h"

struct Batch{
    std::string batchId;
    std::vector<SimulationTeamSetting> teams;
};

#endif