#include <iostream>
#include <cmath>
#include <set>
#include <limits.h>

#include "BattleManager.h"
#include "Battlefield.h"
#include "Constants.h"
#include "TargetManager.h" 
#include "ActionLibrary.h"

// Initialize static member
std::vector<AfterActionEvent> BattleManager::afterActions;

// Static reference for global access
static BattleManager* currentBattleManager = nullptr;

// Global function for ActionLibrary to call
void AddAfterActionToBattleManager(const BattleAction* action, const ActionContext& context) {
    BattleManager::AddAfterAction(action, context);
}

// Global function for ActionLibrary to call DelayUnit
void DelayUnitInBattleManager(Unit* unit, int delayAmount) {
    if (currentBattleManager) {
        currentBattleManager->DelayUnit(unit, delayAmount);
    }
} 


BattleManager::BattleManager(Battlefield& bf) 
    : battlefield(bf) {
    // Set global reference for ActionLibrary access
    currentBattleManager = this;
    
    // Get all units from both teams in the battlefield
    const std::array<Unit*, 5>& redUnits = battlefield.GetRedTeam()->GetUnits();
    const std::array<Unit*, 5>& blueUnits = battlefield.GetBlueTeam()->GetUnits();
    
    // Combine into allUnits vector (total 10 units)
    allUnits.reserve(10);
    allUnits.insert(allUnits.end(), redUnits.begin(), redUnits.end());
    allUnits.insert(allUnits.end(), blueUnits.begin(), blueUnits.end());
}

int BattleManager::CalculateDelayFromDamage(int damageTaken, int maxHP) {
    // Only apply delay if damage was actually taken
    if (damageTaken <= 0) return 0;
    
    float damageRatio = static_cast<float>(damageTaken) / static_cast<float>(maxHP);
    
    // Simple calculation: floor((damage/maxHP) * constant)
    float delayCalculation = damageRatio * DELAY_MULTIPLIER;
    int tickDelay = static_cast<int>(std::floor(delayCalculation));
    
    return tickDelay; // No maximum cap, just return the calculated delay
}

void BattleManager::StartBattle() {
    turnManager.Initialize(allUnits);

    // Reusable vectors for ranged, melee, and magic units
    std::vector<Unit*> rangedUnits;
    std::vector<Unit*> meleeUnits;
    std::vector<Unit*> magicUnits;

    while (!IsBattleOver(false)) {
        // Get all units scheduled to act in the current tick
        std::vector<Unit*> units = turnManager.GetNextUnits();

        if (!units.empty()) {
            std::cout << "[Tick " << turnManager.GetCurrentTick() << "] " << units.size() << " unit(s) acting\n";
        }

        // Separate ranged, melee, and magic units
        rangedUnits.clear();
        meleeUnits.clear();
        magicUnits.clear();

        for (Unit* unit : units) {
            // Ranged vs Melee vs Magic separation
            ActionType actionType = unit->GetWeapon().GetAction().GetActionType();
            if (actionType == ActionType::RANGE) {
                rangedUnits.push_back(unit);
            } else if (actionType == ActionType::MELEE) {
                meleeUnits.push_back(unit);
            } else if (actionType == ActionType::MAGIC) {
                magicUnits.push_back(unit);
            }
        }

        // Combine all acting units and process them one by one
        std::vector<Unit*> allActingUnits;
        allActingUnits.insert(allActingUnits.end(), rangedUnits.begin(), rangedUnits.end());
        allActingUnits.insert(allActingUnits.end(), meleeUnits.begin(), meleeUnits.end());
        allActingUnits.insert(allActingUnits.end(), magicUnits.begin(), magicUnits.end());

        // Get targets for all units at once using smart targeting
        std::map<Unit*, std::vector<Unit*>> allTargets = 
            TargetManager::SelectTargetsForGroup(allActingUnits, allUnits);

        // Track units killed by ranged attacks in this tick (they cannot act)
        std::set<Unit*> unitsKilledByRanged;
        std::set<Unit*> magicUnitsTickResetByRanged;

        // PHASE 1: Execute ranged actions first
        for (Unit* unit : rangedUnits) {
            std::vector<Unit*>& targets = allTargets[unit];
            if (targets.empty()) continue;
            
            Unit* target = targets[0];
            if (!target->IsAlive()) continue;
            
            // Create ally/enemy lists for this unit
            std::vector<Unit*> unitAllies, unitEnemies;
            SplitAlliesAndEnemies(unit, unit->GetWeapon().GetAction(), unitAllies, unitEnemies);
            
            // Execute the ranged action
            std::cout << "Ranged: " << unit->GetName() << " targeting " << target->GetName() 
                      << " (HP: " << target->GetCurrentHP() << ")" << std::endl;
            
            int hpBeforeAttack = target->GetCurrentHP();
            unit->GetWeapon().GetAction().Perform(unit, target, unitAllies, unitEnemies);
            
            // Process any after-actions triggered
            ProcessAfterActions(allUnits);
            
            // Check for speed changes and update turn queue if needed
            turnManager.UpdateSpeedChanges(allUnits);
            
            // Track if target was killed by this ranged attack
            if (hpBeforeAttack > 0 && !target->IsAlive()) {
                unitsKilledByRanged.insert(target);
            }
            
            // If target is still alive and is a magic unit, reset their tick
            if (target->IsAlive() && target->GetWeapon().GetAction().GetActionType() == ActionType::MAGIC) {
                int hpAfterAttack = target->GetCurrentHP();
                int damageTaken = hpBeforeAttack - hpAfterAttack;
                
                if (damageTaken > 0) {
                    turnManager.ResetMagicUnitTick(target);
                    magicUnitsTickResetByRanged.insert(target);  // Track magic units reset by ranged
                    std::cout << "  -> " << target->GetName() << " (magic) tick reset due to ranged attack!" << std::endl;
                }
            }
        }

        // PHASE 2: Execute melee actions 
        for (Unit* unit : meleeUnits) {
            // Melee units can act even if killed by other melee/magic units in same tick
            // Only skip if killed by ranged units in this tick or already dead before this tick
            if (unitsKilledByRanged.count(unit) > 0) continue;
            
            std::vector<Unit*>& targets = allTargets[unit];
            if (targets.empty()) continue;
            
            Unit* target = targets[0];
            if (!target->IsAlive()) continue;
            
            // Create ally/enemy lists for this unit
            std::vector<Unit*> unitAllies, unitEnemies;
            SplitAlliesAndEnemies(unit, unit->GetWeapon().GetAction(), unitAllies, unitEnemies);
            
            // Execute the melee action
            std::cout << "Melee: " << unit->GetName() << " targeting " << target->GetName() 
                      << " (HP: " << target->GetCurrentHP() << ")" << std::endl;
            
            int hpBeforeAttack = target->GetCurrentHP();
            int maxHP = target->GetTotalStat().GetHP(); // Get max HP from totalStat
            
            unit->GetWeapon().GetAction().Perform(unit, target, unitAllies, unitEnemies);
            
            // Process any after-actions triggered
            ProcessAfterActions(allUnits);
            
            // Check for speed changes and update turn queue if needed
            turnManager.UpdateSpeedChanges(allUnits);
            
            // If target is still alive and is a ranged unit, calculate damage-based delay
            if (target->IsAlive() && target->GetWeapon().GetAction().GetActionType() == ActionType::RANGE) {
                int hpAfterAttack = target->GetCurrentHP();
                int damageTaken = hpBeforeAttack - hpAfterAttack;
                
                int delayAmount = CalculateDelayFromDamage(damageTaken, maxHP);
                
                if (delayAmount > 0) {
                    turnManager.DelayUnit(target, delayAmount);
                    std::cout << "  -> " << target->GetName() << " (ranged) delayed by " << delayAmount 
                              << " tick(s) due to " << damageTaken << "/" << maxHP << " damage!" << std::endl;
                }
            }
            
            // If target is still alive and is a magic unit, reset their tick
            if (target->IsAlive() && target->GetWeapon().GetAction().GetActionType() == ActionType::MAGIC) {
                int hpAfterAttack = target->GetCurrentHP();
                int damageTaken = hpBeforeAttack - hpAfterAttack;
                
                if (damageTaken > 0) {
                    turnManager.ResetMagicUnitTick(target);
                    std::cout << "  -> " << target->GetName() << " (magic) tick reset due to melee attack!" << std::endl;
                }
            }
        }

        // PHASE 3: Execute magic actions
        for (Unit* unit : magicUnits) {
            // Magic units can act even if killed by other melee/magic units in same tick
            // Skip if killed by ranged units OR if tick was reset by ranged units in this tick
            if (unitsKilledByRanged.count(unit) > 0 || magicUnitsTickResetByRanged.count(unit) > 0) continue;
            
            std::vector<Unit*>& targets = allTargets[unit];
            if (targets.empty()) continue;
            
            Unit* target = targets[0];
            if (!target->IsAlive()) continue;
            
            // Create ally/enemy lists for this unit
            std::vector<Unit*> unitAllies, unitEnemies;
            SplitAlliesAndEnemies(unit, unit->GetWeapon().GetAction(), unitAllies, unitEnemies);
            
            // Execute the magic action
            std::cout << "Magic: " << unit->GetName() << " targeting " << target->GetName() 
                      << " (HP: " << target->GetCurrentHP() << ")" << std::endl;
            
            unit->GetWeapon().GetAction().Perform(unit, target, unitAllies, unitEnemies);
            
            // Process any after-actions triggered
            ProcessAfterActions(allUnits);
            
            // Check for speed changes and update turn queue if needed
            turnManager.UpdateSpeedChanges(allUnits);
        }

        // Update turn manager with any speed changes that occurred during this tick
        turnManager.UpdateSpeedChanges(allUnits);

        // Advance the tick
        turnManager.AdvanceTick();
    }

    std::cout << "Battle finished.\n";
}



void BattleManager::SplitAlliesAndEnemies(Unit* unit, const BattleAction& action, std::vector<Unit*>& allies, std::vector<Unit*>& enemies) {
    for (Unit* u : allUnits) {
        if (!u->IsAlive()) continue;

        if (u->team == unit->team) {
            if (u == unit && !action.IncludesSelf()) continue;  // skip self if action does not include self
            allies.push_back(u);
        } else {
            enemies.push_back(u);
        }
    }
}


bool BattleManager::IsBattleOver(bool test) {
    if(test) {
        return turnManager.GetCurrentTick() > TEST_ROUND;
    }
    
    // Use battlefield to check for victory
    Team* winner = battlefield.ResultCheck();
    if (winner != nullptr) {
        // Determine team name based on color
        std::string teamName = (winner->GetTeamColor() == Red) ? "Red Team" : "Blue Team";
        std::cout << "Battle Over! " << teamName << " wins!" << std::endl;
        return true;
    }
    
    return false; // Battle continues
}

// Static methods for after-action system
void BattleManager::AddAfterAction(const BattleAction* battleAction, const ActionContext& context) {
    afterActions.emplace_back(battleAction, context);
    std::cout << "[AFTER-ACTION] Registered after-action for " << context.actor->GetName() << std::endl;
}

// Public method to access TurnManager DelayUnit functionality
void BattleManager::DelayUnit(Unit* unit, int delayAmount) {
    turnManager.DelayUnit(unit, delayAmount);
}

void BattleManager::ProcessAfterActions(const std::vector<Unit*>& allUnits) {
    // Move all after-actions to be processed (clear the vector)
    std::vector<AfterActionEvent> toProcess = std::move(afterActions);
    afterActions.clear();
    
    // Process each after-action
    for (const auto& afterActionEvent : toProcess) {
        Unit* actor = afterActionEvent.context.actor;
        
        // Use TargetManager to find the best target based on the action's targeting rules
        Unit* target = TargetManager::FindBestTargetForAction(actor, *afterActionEvent.battleAction, allUnits);
        
        if (target) {
            std::cout << "[AFTER-ACTION] " << actor->GetName() << " performs after-action on " 
                      << target->GetName() << " (threat: " << target->GetTotalStat().GetThreat() << ")" << std::endl;
            
            // Create fresh ally/enemy lists for the after-action
            std::vector<Unit*> allies, enemies;
            for (Unit* unit : allUnits) {
                if (!unit->IsAlive()) continue;
                
                if (unit->team == actor->team) {
                    if (unit == actor && !afterActionEvent.battleAction->IncludesSelf()) continue;
                    allies.push_back(unit);
                } else {
                    enemies.push_back(unit);
                }
            }
            
            // Execute the battle action
            afterActionEvent.battleAction->Perform(actor, target, allies, enemies);
        } else {
            std::cout << "[AFTER-ACTION] No valid target found for " << actor->GetName() << std::endl;
        }
    }
}


