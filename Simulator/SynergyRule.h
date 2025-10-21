#ifndef SYNERGYRULE_H
#define SYNERGYRULE_H

#include <vector>
#include <iostream>
#include <unordered_map>  

#include "Weapon.h"
#include "Armor.h"
#include "Unit.h"
#include "../Element/Stat.h"

struct UnitSynergy
{
Armor* armorptr;
Weapon* weaponptr;
Stat effectStat;
};

struct TeamSynergy
{

};

class SynergyRule
{
public:
static std::unordered_map<std::string, UnitSynergy> unitSynergyMap;

static void ApplyUnitSynergyMatch(Unit* unit);
static void ResetAppliedCounter(); // Add method to reset counter

static int unitSynergyApplied;

static void PrintTotalUnitSynergyApplied();
};



#endif