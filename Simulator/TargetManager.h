#ifndef TARGET_MANAGER_H
#define TARGET_MANAGER_H

#include <vector>
#include <map>
#include "Unit.h"
#include "BattleAction.h"

class TargetManager {
public:
    // Main group targeting method - handles everything
    static std::map<Unit*, std::vector<Unit*>> SelectTargetsForGroup(
        const std::vector<Unit*>& actingUnits,
        const std::vector<Unit*>& allUnits);

    // Legacy single-unit method (for backwards compatibility)
    static std::vector<Unit*> SelectTargets(
        Unit* actingUnit,
        const std::vector<Unit*>& allies,
        const std::vector<Unit*>& enemies,
        const BattleAction& action);

private:
    // Helper: Get valid targets for a specific unit
    static std::vector<Unit*> GetValidTargets(Unit* actingUnit, const std::vector<Unit*>& allUnits);
    
    // Helper: Smart targeting simulation
    static void SimulateGroupTargeting(
        const std::vector<Unit*>& actingUnits,
        const std::vector<Unit*>& allAliveUnits,
        std::map<Unit*, std::vector<Unit*>>& groupTargets);
};

#endif
