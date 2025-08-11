#include "TargetManager.h"
#include <algorithm>
#include <limits.h>

// MAIN METHOD: This is what BattleManager calls
std::map<Unit*, std::vector<Unit*>> TargetManager::SelectTargetsForGroup(
    const std::vector<Unit*>& actingUnits,
    const std::vector<Unit*>& allUnits
) {
    std::map<Unit*, std::vector<Unit*>> groupTargets;
    
    if (actingUnits.empty()) return groupTargets;

    // Step 1: Get all alive units
    std::vector<Unit*> aliveUnits;
    for (Unit* unit : allUnits) {
        if (unit && unit->IsAlive()) {
            aliveUnits.push_back(unit);
        }
    }

    // Step 2: Smart targeting simulation
    SimulateGroupTargeting(actingUnits, aliveUnits, groupTargets);

    return groupTargets;
}

// HELPER: Get valid targets for one unit
std::vector<Unit*> TargetManager::GetValidTargets(Unit* actingUnit, const std::vector<Unit*>& allUnits) {
    std::vector<Unit*> validTargets;
    const BattleAction& action = actingUnit->GetWeapon().GetAction();
    
    for (Unit* target : allUnits) {
        if (!target) continue;
        
        bool canTarget = false;
        
        if (action.GetTargetType() == TargetType::ALLY) {
            // Can target teammates
            if (target->team == actingUnit->team) {
                if (target != actingUnit || action.IncludesSelf()) {
                    canTarget = true;
                }
            }
        } else {
            // Can target enemies  
            if (target->team != actingUnit->team) {
                canTarget = true;
            }
        }
        
        if (canTarget) {
            validTargets.push_back(target);
        }
    }
    
    return validTargets;
}

// HELPER: Smart targeting simulation with shared HP
void TargetManager::SimulateGroupTargeting(
    const std::vector<Unit*>& actingUnits,
    const std::vector<Unit*>& allAliveUnits,
    std::map<Unit*, std::vector<Unit*>>& groupTargets
) {
    // Create shared HP simulation
    std::map<Unit*, int> simulatedHP;
    for (Unit* unit : allAliveUnits) {
        simulatedHP[unit] = unit->GetCurrentHP();
    }

    // Each unit selects targets using shared simulation
    for (Unit* actingUnit : actingUnits) {
        // Get this unit's valid targets
        std::vector<Unit*> validTargets = GetValidTargets(actingUnit, allAliveUnits);
        
        // Find best target that's still alive in simulation (prioritize highest threat)
        Unit* bestTarget = nullptr;
        int highestThreat = INT_MIN;  // Changed from -1 to handle negative threat values
        
        for (Unit* target : validTargets) {
            if (simulatedHP[target] > 0) {
                int targetThreat = target->GetTotalStat().GetThreat();
                if (targetThreat > highestThreat) {
                    highestThreat = targetThreat;
                    bestTarget = target;
                }
            }
        }
        
        // If we found a target, simulate damage
        if (bestTarget) {
            const BattleAction& action = actingUnit->GetWeapon().GetAction();
            int damage = action.CalculateDamage(actingUnit, bestTarget);
            
            // Apply damage to simulation
            simulatedHP[bestTarget] -= damage;
            if (simulatedHP[bestTarget] < 0) {
                simulatedHP[bestTarget] = 0;
            }
            
            // Assign target
            groupTargets[actingUnit] = {bestTarget};
        } else {
            // No valid targets
            groupTargets[actingUnit] = {};
        }
    }
}

// LEGACY METHOD: For backwards compatibility
std::vector<Unit*> TargetManager::SelectTargets(
    Unit* actingUnit,
    const std::vector<Unit*>& allies,
    const std::vector<Unit*>& enemies,
    const BattleAction& action
) {
    std::vector<Unit*> candidates;

    // Determine target pool
    if (action.GetTargetType() == TargetType::ALLY) {
        candidates = allies;
    } else {
        candidates = enemies;
    }

    // Handle self-targeting
    if (!action.IncludesSelf()) {
        candidates.erase(
            std::remove(candidates.begin(), candidates.end(), actingUnit),
            candidates.end()
        );
    }

    // Filter alive only
    std::vector<Unit*> aliveTargets;
    for (Unit* candidate : candidates) {
        if (candidate->IsAlive()) {
            aliveTargets.push_back(candidate);
        }
    }

    // Return first available target
    return aliveTargets.empty() ? std::vector<Unit*>{} : std::vector<Unit*>{aliveTargets[0]};
}
