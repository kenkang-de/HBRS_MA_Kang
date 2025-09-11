#ifndef BATCH_H
#define BATCH_H

#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"
#include "../Simulator/Unit.h"

class Batch{
public:
    std::vector<Team> teams; 
    std::string batchId;
};

#endif