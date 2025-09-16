#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <array>
#include <vector>

#include "Unit.h"
#include "Battlefield.h"
#include "../Sampling/Batch.h"

#include "../Element/ElementList.h"
#include "../Sampling/SimulationTeamSetting.h"

class Simulator
{
private:
ElementList* elementList;
std::unordered_map<std::string, BattleAction>* actionMap;
std::array<Unit,10>* battleUnits;

std::unique_ptr<Battlefield> battlefield;

public:
Simulator(ElementList* elementList, std::unordered_map<std::string, BattleAction>* actionMap , std::array<Unit,10>* battleUnits){
    this->elementList = elementList;
    this->actionMap = actionMap;
    this->battleUnits = battleUnits;
    Init_Battlefield();
};

void Init_Battlefield();

void SimulateBatches(std::vector<Batch>* batches);
void SimulateBattle(SimulationTeamSetting* redTeamSetting, SimulationTeamSetting* blueTeamSetting,bool log,std::string batchID);
void EquipTeam(int startIndex, SimulationTeamSetting* teamSetting);

};

#endif