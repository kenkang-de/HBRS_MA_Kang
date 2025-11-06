#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <set>
#include <streambuf> // Add this for stream redirection

#include "../Constants.h"
#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "BattleManager.h"
#include "Battlefield.h"
#include "GlobalAction.h"
#include "TargetManager.h"

// Static reference for global access
static BattleManager *currentBattleManager = nullptr;

BattleManager::BattleManager(Battlefield &bf) : battlefield(bf) {
    GlobalAction::ClearAfterAction();
    currentBattleManager = this;

    const std::array<Unit *, 5> &redUnits = battlefield.GetRedTeam()->GetUnits();
    const std::array<Unit *, 5> &blueUnits = battlefield.GetBlueTeam()->GetUnits();

    allUnits.reserve(10);
    allUnits.insert(allUnits.end(), redUnits.begin(), redUnits.end());
    allUnits.insert(allUnits.end(), blueUnits.begin(), blueUnits.end());
}

BattleManager::~BattleManager() {
    if (currentBattleManager == this) {
        currentBattleManager = nullptr;
    }
    GlobalAction::ClearAfterAction();

    allUnits.clear(); // Clear unit pointers
}

int BattleManager::CalculateDelayFromDamage(int damageTaken, int maxHP) {
    // Only apply delay if damage was actually taken
    if (damageTaken <= 0)
        return 0;

    float damageRatio = static_cast<float>(damageTaken) / static_cast<float>(maxHP);

    // Simple calculation: floor((damage/maxHP) * constant)
    float delayCalculation = damageRatio * DELAY_MULTIPLIER;
    int tickDelay = static_cast<int>(std::floor(delayCalculation));

    return tickDelay;
}

void BattleManager::StartBattle(bool log, std::string batchID) {

    turnManager.Initialize(allUnits);

    if (log) {
        std::string logFilename = Paths::LOG_V1_DIR + batchID + ".txt";
        LogSystem::StartLogging(logFilename);
    }

    LogSystem::LogUnitListStats(allUnits);

    // Reusable vectors for ranged, melee, and magic units
    std::vector<Unit *> rangedUnits;
    std::vector<Unit *> meleeUnits;
    std::vector<Unit *> magicUnits;

    rangedUnits.reserve(10);
    meleeUnits.reserve(10);
    magicUnits.reserve(10);

    while (!IsBattleOver()) {
        // Get all units scheduled to act in the current tick
        std::vector<Unit *> units = turnManager.GetNextUnits();

        if (!units.empty()) {
            LogSystem::Log("\n");
            LogSystem::LogStream("[Tick ", turnManager.GetCurrentTick(), "] ", LogSystem::GetUnitListName(units),
                                 " acting");
        } else {
            // Check if queue is empty and no units can act (battle should end)
            if (!turnManager.CanContinue(allUnits)) {
                LogSystem::LogStream("DRAW");
                LogDrawResult();
                LogUsageCount();
                break;
            }
        }

        LogSystem::LogSurvivedUnit(allUnits);

        rangedUnits.clear();
        meleeUnits.clear();
        magicUnits.clear();

        for (Unit *unit : units) {
            ActionType actionType = unit->GetWeapon()->GetAction().GetActionType();
            if (actionType == ActionType::RANGE) {
                rangedUnits.push_back(unit);
            } else if (actionType == ActionType::MELEE) {
                meleeUnits.push_back(unit);
            } else if (actionType == ActionType::MAGIC) {
                magicUnits.push_back(unit);
            }
        }

        // map of acting unit and list of targets
        std::map<Unit *, std::vector<Unit *>> allTargets = TargetManager::SelectTargetsForGroup(units, allUnits);

        // Track units killed by ranged attacks in this tick (they cannot act)
        std::set<Unit *> unitsKilledByRanged;
        std::set<Unit *> magicUnitsTickResetByRanged;

        // RangeUnit
        ActUnits(rangedUnits, allTargets);

        turnManager.RevalidateEntry(meleeUnits);
        turnManager.RevalidateEntry(magicUnits);

        // Melee
        ActUnits(meleeUnits, allTargets);

        // Magic
        ActUnits(magicUnits, allTargets);
    }

    if (log) {
        LogSystem::StopLogging();
    }
}

void BattleManager::ActUnits(std::vector<Unit *> &units, std::map<Unit *, std::vector<Unit *>> &allTargets) {
    for (Unit *unit : units) {

        std::vector<Unit *> &targets = allTargets[unit];
        for (Unit *target : targets) {
            unit->GetWeapon()->GetAction().Perform(unit, target, unit->Allies, unit->Enemies);

            // Apply unit boons to after-action system
            ApplyUnitBoonsToAfterAction(unit);
            // Process any after-actions triggered
            ProcessAfterActions(allUnits);
        }
        // Check for speed changes and update turn queue if needed
        // turnManager.UpdateSpeedChanges();
    }
}

bool BattleManager::IsBattleOver() {
    if (turnManager.GetCurrentTick() >= 500) {
        LogSystem::LogStream("Battle Over! Tick limit reached (", turnManager.GetCurrentTick(), " ticks elapsed)");
        LogDrawResult();
        LogUsageCount();
        return true;
    }

    // Use battlefield to check for victory
    Team *winner = battlefield.GetWinnerTeam();
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

void BattleManager::LogDrawResult() {
    for (Unit *unit : allUnits) {
        unit->GetWeapon()->recordDraw();
        unit->GetArmor()->recordDraw();
    }
}

void BattleManager::LogWinLoseResult() {
    Team *winnerTeam = battlefield.GetWinnerTeam();
    Team *loserTeam = battlefield.GetLoserTeam();

    for (Unit *unit : winnerTeam->GetUnits()) {
        unit->GetWeapon()->recordWin();
        unit->GetArmor()->recordWin();
    }

    for (Unit *unit : loserTeam->GetUnits()) {
        unit->GetWeapon()->recordLoss();
        unit->GetArmor()->recordLoss();
    }
}

void BattleManager::LogUsageCount() {
    for (Unit *unit : allUnits) {
        unit->GetWeapon()->incrementUsage();
        unit->GetArmor()->incrementUsage();
    }
}

// Public method to access TurnManager DelayUnit functionality
void BattleManager::DelayUnit(Unit *unit, int delayAmount) {
    turnManager.DelayUnit(unit, delayAmount);
}

void BattleManager::ProcessAfterActions(const std::vector<Unit *> &allUnits) {
    // Move all after-actions to be processed (clear the vector)
    std::vector<AfterActionEvent> toProcess = std::move(GlobalAction::afterActions);

    // Process each after-action
    for (const auto &afterActionEvent : toProcess) {
        Unit *actor = afterActionEvent.context.actor;

        // Use the target from context if specified, otherwise find a target
        Unit *target = afterActionEvent.context.target;
        if (!target || !target->IsAlive()) {
            // Use TargetManager to find the best target based on the action's targeting rules
            target = TargetManager::FindBestTargetForAction(actor, *afterActionEvent.battleAction, allUnits);
        }

        if (target) {
            LogSystem::LogStream("[AFTER-ACTION] ", actor->GetName(), " performs after-action on ", target->GetName(),
                                 " (threat: ", target->GetTotalStat().GetThreat(), ")");

            // Create fresh ally/enemy lists for the after-action
            std::vector<Unit *> allies, enemies;
            for (Unit *unit : allUnits) {
                if (!unit->IsAlive())
                    continue;

                if (unit->team == actor->team) {
                    if (unit == actor && !afterActionEvent.battleAction->IncludesSelf())
                        continue;
                    allies.push_back(unit);
                } else {
                    enemies.push_back(unit);
                }
            }

            // Execute the battle action
            afterActionEvent.battleAction->Perform(actor, target, allies, enemies);
        } else {
            LogSystem::LogStream("[AFTER-ACTION] No valid target found for ", actor->GetName());
        }
    }
}

// Apply unit boons to after-action system
void BattleManager::ApplyUnitBoonsToAfterAction(Unit *unit) {
    if (unit->IsAlive()) {
        // The unit applies its boons to the after-action system
        unit->ApplyBoonsToAfterAction();
    }
}
