#include "SynergyRule.h"
#include "../Log/LogSystem.h"

std::unordered_map<std::string, UnitSynergy> SynergyRule::unitSynergyMap;
int SynergyRule::unitSynergyApplied = 0;

void SynergyRule::ApplyUnitSynergyMatch(Unit *unit) {
    std::vector<std::string> armorUnitSynergyIDs = unit->GetArmor()->GetUnitSynergyIDs();
    std::vector<std::string> weaponUnitSynergyIDs = unit->GetWeapon()->GetUnitSynergyIDs();

    // Early exit if either list is empty to avoid unnecessary nested loops
    if (armorUnitSynergyIDs.empty() || weaponUnitSynergyIDs.empty()) {
        return;
    }

    for (const std::string &armorUnitSynergyID : armorUnitSynergyIDs) {
        for (const std::string &weaponUnitSynergyID : weaponUnitSynergyIDs) {
            if (armorUnitSynergyID == weaponUnitSynergyID) {
                auto it = SynergyRule::unitSynergyMap.find(armorUnitSynergyID);
                if (it != SynergyRule::unitSynergyMap.end()) {
                    // Apply synergy effect
                    unit->Synergy = true;
                    SynergyRule::unitSynergyApplied++;
                }
            }
        }
    }
}

void SynergyRule::PrintTotalUnitSynergyApplied() {
    std::cout << "Total unit synergy applied: " << SynergyRule::unitSynergyApplied << std::endl;
}

void SynergyRule::ResetAppliedCounter() {
    unitSynergyApplied = 0;
}