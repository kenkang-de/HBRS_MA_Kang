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

    // Helper methods for after-action system
    static Unit* FindHighestThreatTarget(const std::vector<Unit*>& candidates);
    static Unit* FindBestTargetForAction(Unit* actor, const BattleAction& action, const std::vector<Unit*>& allUnits);

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
