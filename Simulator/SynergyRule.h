#ifndef SYNERGYRULE_H
#define SYNERGYRULE_H

#include <iostream>
#include <unordered_map>
#include <vector>

#include "../Element/Stat.h"
#include "Armor.h"
#include "Unit.h"
#include "Weapon.h"

struct UnitSynergy {
    Armor *armorptr;
    Weapon *weaponptr;
    Stat effectStat;
};

struct TeamSynergy {};

class SynergyRule {
  public:
    static std::unordered_map<std::string, UnitSynergy> unitSynergyMap;

    static void ApplyUnitSynergyMatch(Unit *unit);
    static void ResetAppliedCounter();

    static int unitSynergyApplied;

    static void PrintTotalUnitSynergyApplied();
};

#endif