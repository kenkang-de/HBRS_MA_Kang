#include <iostream>
#include <cmath>

#include "BattleManager.h"
#include "Battlefield.h"
#include "Constants.h"
#include "TargetManager.h" 


BattleManager::BattleManager(Battlefield& bf) 
    : battlefield(bf) {
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

    // Reusable vectors for ranged and melee units
    std::vector<Unit*> rangedUnits;
    std::vector<Unit*> meleeUnits;

    while (!IsBattleOver(false)) {
        // Get all units scheduled to act in the current tick
        std::vector<Unit*> units = turnManager.GetNextUnits();

        if (!units.empty()) {
            std::cout << "[Tick " << turnManager.GetCurrentTick() << "] " << units.size() << " unit(s) acting\n";
        }

        // Separate ranged and melee units
        rangedUnits.clear();
        meleeUnits.clear();

        for (Unit* unit : units) {
            // Ranged vs Melee separation
            if (unit->GetWeapon().GetAction().GetActionType() == ActionType::RANGE) {
                rangedUnits.push_back(unit);
            } else {
                meleeUnits.push_back(unit);
            }
        }

        // Combine all acting units and process them one by one
        std::vector<Unit*> allActingUnits;
        allActingUnits.insert(allActingUnits.end(), rangedUnits.begin(), rangedUnits.end());
        allActingUnits.insert(allActingUnits.end(), meleeUnits.begin(), meleeUnits.end());

        // Get targets for all units at once using smart targeting
        std::map<Unit*, std::vector<Unit*>> allTargets = 
            TargetManager::SelectTargetsForGroup(allActingUnits, allUnits);

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
            
            unit->GetWeapon().GetAction().Perform(unit, target, unitAllies, unitEnemies);
        }

        // PHASE 2: Execute melee actions (only for units still alive)
        for (Unit* unit : meleeUnits) {
            // Check if this melee unit is still alive after ranged phase
            if (!unit->IsAlive()) continue;
            
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
