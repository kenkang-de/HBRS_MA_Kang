#ifndef SIM_TEAM_SETTING_H
#define SIM_TEAM_SETTING_H

#include <array>

#include "SimulationUnitSetting.h"

struct SimulationTeamSetting
{
std::array<SimulationUnitSetting, 5> simulationUnitSettings;
};

#endif 