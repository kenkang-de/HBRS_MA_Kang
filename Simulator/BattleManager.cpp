#include <iostream>
#include <cmath>
#include <set>
#include <limits.h>
#include <ctime>
#include <fstream>
#include <streambuf>  // Add this for stream redirection

#include "../Log/LogSystem.h"
#include "BattleManager.h"
#include "Battlefield.h"
#include "Constants.h"
#include "TargetManager.h" 
#include "ActionLibrary.h"
#include "GlobalAction.h"

// Static reference for global access
static BattleManager* currentBattleManager = nullptr;

BattleManager::BattleManager(Battlefield& bf) 
    : battlefield(bf), tickCount(0) {

    GlobalAction::ClearAfterAction();
    currentBattleManager = this;
    
    const std::array<Unit*, 5>& redUnits = battlefield.GetRedTeam()->GetUnits();
    const std::array<Unit*, 5>& blueUnits = battlefield.GetBlueTeam()->GetUnits();
    
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
    
    return tickDelay;
}

void BattleManager::StartBattle(bool log, std::string batchID) {
    std::cout<<"[BattleManager] StartBattle called with log="<<(log?"true":"false")<<", batchID="<<batchID<<std::endl;

    std::cout<<"[BattleManager] Initializing turn manager..."<<std::endl;
    turnManager.Initialize(allUnits);
    std::cout<<"[BattleManager] Turn manager initialized"<<std::endl;

    if(log) {
        std::cout<<"[BattleManager] Starting logging to file..."<<std::endl;
        std::string logFilename = Paths::LOG_V1_DIR + batchID + ".txt";
        LogSystem::StartLogging(logFilename);
        std::cout<<"[BattleManager] Logging started"<<std::endl;
    }

    std::cout<<"[BattleManager] Checking unit states..."<<std::endl;
    // Debug: Check unit states before battle
    LogSystem::LogStream("=== BATTLE START DEBUG ===");
    for (int i = 0; i < allUnits.size(); i++) {
        Unit* unit = allUnits[i];
        LogSystem::LogStream("Unit ", i, ": ", unit->GetName(), 
                           " HP:", unit->GetCurrentHP(), 
                           " Speed:", unit->GetTotalStat().GetSpeed(),
                           " Alive:", (unit->IsAlive() ? "Yes" : "No"));
    }
    LogSystem::LogStream("==========================");
    std::cout<<"[BattleManager] Unit states checked"<<std::endl;

    // Reusable vectors for ranged, melee, and magic units
    std::vector<Unit*> rangedUnits;
    std::vector<Unit*> meleeUnits;
    std::vector<Unit*> magicUnits;

    std::cout<<"[BattleManager] Starting main battle loop..."<<std::endl;
    while (!IsBattleOver(false)) {
        // Get all units scheduled to act in the current tick
        std::vector<Unit*> units = turnManager.GetNextUnits();

        if (!units.empty()) {
            LogSystem::Log("\n");
            LogSystem::LogStream("[Tick ", turnManager.GetCurrentTick(), "] ", units.size(), " unit(s) acting");
        }
        else
        {
            turnManager.AdvanceTick();
            continue;
        }

        // Separate ranged, melee, and magic units
        rangedUnits.clear();
        meleeUnits.clear();
        magicUnits.clear();

        for (Unit* unit : units) {
            // Ranged vs Melee vs Magic separation
            if (!unit->GetWeapon()) continue;
            ActionType actionType = unit->GetWeapon()->GetAction().GetActionType();
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
            
            // Execute action against all targets
            for (Unit* target : targets) {
                if (!target->IsAlive()) continue;
                
                // Skip if unit has no weapon
                if (!unit->GetWeapon()) continue;
                
                // Create ally/enemy lists for this unit
                std::vector<Unit*> unitAllies, unitEnemies;
                SplitAlliesAndEnemies(unit, unit->GetWeapon()->GetAction(), unitAllies, unitEnemies);
                
                LogSystem::LogStream("Ranged: ", unit->GetName(), " targeting ", target->GetName(), 
                                " (HP: ", target->GetCurrentHP(), ")");
           
                
                int hpBeforeAttack = target->GetCurrentHP();
                
                // Check if unit is frozen - if so, skip the action but still apply boons
                if (!unit->IsFrozen()) {
                    unit->GetWeapon()->GetAction().Perform(unit, target, unitAllies, unitEnemies);
                } else {
                    LogSystem::LogStream("  -> ", unit->GetName(), " is frozen and cannot act!");
                }
                
                // Apply unit boons to after-action system
                ApplyUnitBoonsToAfterAction(unit);
                
                // Process any after-actions triggered
                ProcessAfterActions(allUnits);
                
                // Check for speed changes and update turn queue if needed
                turnManager.UpdateSpeedChanges(allUnits);
                
                // Track if target was killed by this ranged attack
                if (hpBeforeAttack > 0 && !target->IsAlive()) {
                    unitsKilledByRanged.insert(target);
                }
                
                // If target is still alive and is a magic unit, reset their tick
                if (target->IsAlive() && target->GetWeapon() && target->GetWeapon()->GetAction().GetActionType() == ActionType::MAGIC) {
                    int hpAfterAttack = target->GetCurrentHP();
                    int damageTaken = hpBeforeAttack - hpAfterAttack;
                    
                    if (damageTaken > 0) {
                        turnManager.ResetMagicUnitTick(target);
                        magicUnitsTickResetByRanged.insert(target);
                        LogSystem::LogStream("  -> ", target->GetName(), " (magic) tick reset due to ranged attack!");
                    }
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
            
            // Execute action against all targets
            for (Unit* target : targets) {
                if (!target->IsAlive()) continue;
                
                // Create ally/enemy lists for this unit
                std::vector<Unit*> unitAllies, unitEnemies;
                SplitAlliesAndEnemies(unit, unit->GetWeapon()->GetAction(), unitAllies, unitEnemies);
                
                LogSystem::LogStream("Melee: ", unit->GetName(), " targeting ", target->GetName(), 
                                    " (HP: ", target->GetCurrentHP(), ")");

                
                int hpBeforeAttack = target->GetCurrentHP();
                int maxHP = target->GetTotalStat().GetHP(); // Get max HP from totalStat
                
                // Check if unit is frozen - if so, skip the action but still apply boons
                if (!unit->IsFrozen()) {
                    unit->GetWeapon()->GetAction().Perform(unit, target, unitAllies, unitEnemies);
                } else {
                    LogSystem::LogStream("  -> ", unit->GetName(), " is frozen and cannot act!");
                }
                
                // Apply unit boons to after-action system
                ApplyUnitBoonsToAfterAction(unit);
                
                // Process any after-actions triggered
                ProcessAfterActions(allUnits);
                
                // Check for speed changes and update turn queue if needed
                turnManager.UpdateSpeedChanges(allUnits);
                
                // If target is still alive and is a ranged unit, calculate damage-based delay
                if (target->IsAlive() && target->GetWeapon() && target->GetWeapon()->GetAction().GetActionType() == ActionType::RANGE) {
                    int hpAfterAttack = target->GetCurrentHP();
                    int damageTaken = hpBeforeAttack - hpAfterAttack;
                    
                    int delayAmount = CalculateDelayFromDamage(damageTaken, maxHP);
                    
                    if (delayAmount > 0) {
                        turnManager.DelayUnit(target, delayAmount);
                        LogSystem::LogStream("  -> ", target->GetName(), " (ranged) delayed by ", delayAmount, 
                                            " tick(s) due to ", damageTaken, "/", maxHP, " damage!");
                    }
                }
                
                // If target is still alive and is a magic unit, reset their tick
                if (target->IsAlive() && target->GetWeapon() && target->GetWeapon()->GetAction().GetActionType() == ActionType::MAGIC) {
                    int hpAfterAttack = target->GetCurrentHP();
                    int damageTaken = hpBeforeAttack - hpAfterAttack;
                    
                    if (damageTaken > 0) {
                        turnManager.ResetMagicUnitTick(target);
                        LogSystem::LogStream("  -> ", target->GetName(), " (magic) tick reset due to melee attack!");
                    }
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
            
            // Execute action against all targets
            for (Unit* target : targets) {
                if (!target->IsAlive()) continue;
                
                // Create ally/enemy lists for this unit
                std::vector<Unit*> unitAllies, unitEnemies;
                SplitAlliesAndEnemies(unit, unit->GetWeapon()->GetAction(), unitAllies, unitEnemies);
                
                LogSystem::LogStream("Magic: ", unit->GetName(), " targeting ", target->GetName(), 
                                    " (HP: ", target->GetCurrentHP(), ")");
           
                
                // Check if unit is frozen - if so, skip the action but still apply boons
                if (!unit->IsFrozen()) {
                    unit->GetWeapon()->GetAction().Perform(unit, target, unitAllies, unitEnemies);
                } else {
                    LogSystem::LogStream("  -> ", unit->GetName(), " is frozen and cannot act!");
                }
                
                // Apply unit boons to after-action system
                ApplyUnitBoonsToAfterAction(unit);
                
                // Process any after-actions triggered
                ProcessAfterActions(allUnits);
                
                // Check for speed changes and update turn queue if needed
                turnManager.UpdateSpeedChanges(allUnits);
            }
            
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
    
    std::cout<<"[BattleManager] Battle loop ended"<<std::endl;
    std::cout<<"[BattleManager] Finalizing battle..."<<std::endl;

    if(log) {
        LogSystem::StopLogging();
    }
    std::cout<<"[BattleManager] StartBattle method completed!"<<std::endl;
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
    // Check if we've reached the tick limit based on actual turn manager ticks
    int currentTick = turnManager.GetCurrentTick();
    if (currentTick >= TEST_TICK) {
        LogSystem::LogStream("Battle Over! Tick limit reached (", currentTick, " ticks elapsed)");
        LogDrawResult();
        LogUsageCount();
        return true;
    }
    
    // Use battlefield to check for victory
    Team* winner = battlefield.GetWinnerTeam();
    if (winner != nullptr) {
        // Determine team name based on color
        std::string teamName = (winner->GetTeamColor() == Red) ? "Red Team" : "Blue Team";
        LogSystem::LogStream("Battle Over! ", teamName, " wins!");
        LogWinLoseResult();
        LogUsageCount();
        return true;
    }
    return false; // Battle continues
}

void BattleManager::LogDrawResult()
{
    for(Unit* unit: allUnits)
    {
        unit->GetWeapon()->recordDraw();
        unit->GetArmor()->recordDraw();
    }
}

void BattleManager::LogWinLoseResult()
{
Team* winnerTeam = battlefield.GetWinnerTeam();
Team* loserTeam = battlefield.GetLoserTeam();

for(Unit* unit: winnerTeam->GetUnits()){
    unit->GetWeapon()->recordWin();
    unit->GetArmor()->recordWin();
}

for(Unit* unit: loserTeam->GetUnits()){
    unit->GetWeapon()->recordLoss();
    unit->GetArmor()->recordLoss();
}
}

void BattleManager::LogUsageCount()
{
    for(Unit* unit: allUnits){
        unit->GetWeapon()->incrementUsage();
        unit->GetArmor()->incrementUsage();
    }
}



// Public method to access TurnManager DelayUnit functionality
void BattleManager::DelayUnit(Unit* unit, int delayAmount) {
    turnManager.DelayUnit(unit, delayAmount);
}

void BattleManager::ProcessAfterActions(const std::vector<Unit*>& allUnits) {
    // Move all after-actions to be processed (clear the vector)
    std::vector<AfterActionEvent> toProcess = std::move(GlobalAction::afterActions);
    
    // Process each after-action
    for (const auto& afterActionEvent : toProcess) {
        Unit* actor = afterActionEvent.context.actor;
        
        // Use the target from context if specified, otherwise find a target
        Unit* target = afterActionEvent.context.target;
        if (!target || !target->IsAlive()) {
            // Use TargetManager to find the best target based on the action's targeting rules
            target = TargetManager::FindBestTargetForAction(actor, *afterActionEvent.battleAction, allUnits);
        }
        
        if (target) {
            LogSystem::LogStream("[AFTER-ACTION] " , actor->GetName() , " performs after-action on " 
                      , target->GetName() , " (threat: " , target->GetTotalStat().GetThreat() , ")");
            
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
            LogSystem::LogStream( "[AFTER-ACTION] No valid target found for " , actor->GetName());
        }
    }
}

// Apply unit boons to after-action system
void BattleManager::ApplyUnitBoonsToAfterAction(Unit* unit) {
    if (unit->IsAlive()) {
        // The unit applies its boons to the after-action system
        unit->ApplyBoonsToAfterAction();
    }
}


