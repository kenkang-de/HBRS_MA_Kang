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
    if (!actingUnit->GetWeapon()) return validTargets;
    const BattleAction& action = actingUnit->GetWeapon()->GetAction();
    
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
        
        // Filter valid targets by simulation (only alive in simulation)
        std::vector<Unit*> aliveInSimulation;
        for (Unit* target : validTargets) {
            if (simulatedHP[target] > 0) {
                aliveInSimulation.push_back(target);
            }
        }
        
        // Find best target that's still alive in simulation (prioritize highest threat)
        Unit* bestTarget = FindHighestThreatTarget(aliveInSimulation);
        
        // If we found a target, simulate damage and potentially select more targets
        if (bestTarget) {
            if (!actingUnit->GetWeapon()) {
                groupTargets[actingUnit] = {};
                continue;
            }
            const BattleAction& action = actingUnit->GetWeapon()->GetAction();
            std::vector<Unit*> selectedTargets = {bestTarget};
            
            // Only apply damage simulation if this is a damage-dealing action
            // Pure debuff spells (like Corrosion with negative attack) don't need HP simulation
            bool isDamageAction = (actingUnit->GetTotalStat().GetAttack() > 0);
            
            if (isDamageAction) {
                // Apply damage to simulation for first target
                int damage = action.CalculateDamage(actingUnit, bestTarget);
                simulatedHP[bestTarget] -= damage;
                if (simulatedHP[bestTarget] < 0) {
                    simulatedHP[bestTarget] = 0;
                }
            }
            
            // Select additional targets if targetNumber > 1
            int targetNumber = action.GetTargetNumber();
            for (int i = 1; i < targetNumber; i++) {
                // Re-filter alive targets after previous selections
                std::vector<Unit*> remainingTargets;
                for (Unit* target : validTargets) {
                    if (simulatedHP[target] > 0 && 
                        std::find(selectedTargets.begin(), selectedTargets.end(), target) == selectedTargets.end()) {
                        remainingTargets.push_back(target);
                    }
                }
                
                Unit* nextTarget = FindHighestThreatTarget(remainingTargets);
                if (nextTarget) {
                    selectedTargets.push_back(nextTarget);
                    
                    if (isDamageAction) {
                        // Apply damage to simulation
                        int nextDamage = action.CalculateDamage(actingUnit, nextTarget);
                        simulatedHP[nextTarget] -= nextDamage;
                        if (simulatedHP[nextTarget] < 0) {
                            simulatedHP[nextTarget] = 0;
                        }
                    }
                } else {
                    break; // No more valid targets
                }
            }
            
            // Assign all selected targets
            groupTargets[actingUnit] = selectedTargets;
        } else {
            // No valid targets
            groupTargets[actingUnit] = {};
        }
    }
}

// Helper method: Find target with highest threat value
Unit* TargetManager::FindHighestThreatTarget(const std::vector<Unit*>& candidates) {
    Unit* bestTarget = nullptr;
    int highestThreat = INT_MIN;
    
    for (Unit* candidate : candidates) {
        if (candidate && candidate->IsAlive()) {
            int targetThreat = candidate->GetTotalStat().GetThreat();
            if (targetThreat > highestThreat) {
                highestThreat = targetThreat;
                bestTarget = candidate;
            }
        }
    }
    
    return bestTarget;
}

// Helper method: Find best target for a specific action (supports allies, enemies, self-targeting)
Unit* TargetManager::FindBestTargetForAction(Unit* actor, const BattleAction& action, const std::vector<Unit*>& allUnits) {
    // Get all valid targets based on the action's targeting rules
    std::vector<Unit*> validTargets = GetValidTargets(actor, allUnits);
    
    // Find the highest threat target among valid targets
    return FindHighestThreatTarget(validTargets);
}
