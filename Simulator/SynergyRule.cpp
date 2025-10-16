#include "SynergyRule.h"
#include "../Log/LogSystem.h"

std::unordered_map<std::string, UnitSynergy> SynergyRule::unitSynergyMap;
int SynergyRule::unitSynergyApplied = 0;

void SynergyRule::ApplyUnitSynergyMatch(Unit* unit)
{
    std::vector<std::string> armorUnitSynergyIDs = unit->GetArmor()->GetUnitSynergyIDs();
    std::vector<std::string> weaponUnitSynergyIDs = unit->GetWeapon()->GetUnitSynergyIDs();

    for (std::string& armorUnitSynergyID : armorUnitSynergyIDs) {
        for (std::string& weaponUnitSynergyID : weaponUnitSynergyIDs) {
            if (armorUnitSynergyID == weaponUnitSynergyID) {
                UnitSynergy unitSynergy = SynergyRule::unitSynergyMap[armorUnitSynergyID];
                    // Apply synergy effect
                    unit->GetTotalStat() += unitSynergy.effectStat;
                    SynergyRule::unitSynergyApplied++;
            }
        }
    }
}

void SynergyRule::PrintTotalUnitSynergyApplied()
{
    std::cout<<"Total unit synergy applied: "<< SynergyRule::unitSynergyApplied << std::endl;
}