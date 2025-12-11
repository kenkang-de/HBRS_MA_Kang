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

    TickCounted = 0;

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
            } else {
                // when there are units that cannot act in this turn but scheduled before returns 0 from GetNextUnits()
                continue;
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
        // RangeUnit
        ActUnits(rangedUnits);

        turnManager.RevalidateEntry(meleeUnits);
        turnManager.RevalidateEntry(magicUnits);

        // Melee
        ActUnits(meleeUnits);

        // Magic
        ActUnits(magicUnits);

        TickCounted++;
    }

    if (log) {
        LogSystem::StopLogging();
    }
}

void BattleManager::ActUnits(std::vector<Unit *> &units) {

    // normal actions
    for (Unit *actor : units) {
        std::vector<Unit *> targets = TargetManager::GetTargets(*actor);
        for (Unit *target : targets) {
            BattleAction &action = actor->GetWeapon()->GetAction();

            LogSystem::LogStream(actor->GetName(), "(", actor->GetTotalStat().GetAttack(), ",",
                                 actor->GetTotalStat().GetDefense(), ",", actor->GetTotalStat().GetHP(), ",",
                                 actor->GetTotalStat().GetSpeed(), ",", actor->GetTotalStat().GetThreat(), ")",
                                 " performs [", action.GetID(), "] to " + target->GetName(), "(",
                                 target->GetTotalStat().GetAttack(), ",", target->GetTotalStat().GetDefense(), ",",
                                 target->GetTotalStat().GetHP(), ",", target->GetTotalStat().GetSpeed(), ",",
                                 target->GetTotalStat().GetThreat(), ")");
            action.Perform(actor, target);
        }
    }
    // after actions
    for (Unit *actor : units) {
        // Apply unit boons to after-action system
        ApplyUnitBoonsToAfterAction(actor);
    }
    // Process any after-actions triggered
    ProcessAfterActions(allUnits);
}

bool BattleManager::IsBattleOver() {
    // Check tick limit
    if (TickCounted >= 100) {
        LogSystem::LogStream("Battle Over! Tick limit reached");
        LogDrawResult();
        LogUsageCount();
        return true;
    }

    BATTLERESULT result = battlefield.GetBattleResult();

    switch (result) {
    case DRAW:
        LogSystem::LogStream("Battle Over! Draw - both teams eliminated!");
        LogDrawResult();
        LogUsageCount();
        return true;

    case REDWIN:
        LogSystem::LogStream("Battle Over! Red Team wins!");
        LogWinLoseResult(result);
        LogUsageCount();
        return true;

    case BLUEWIN:
        LogSystem::LogStream("Battle Over! Blue Team wins!");
        LogWinLoseResult(result);
        LogUsageCount();
        return true;

    case ONGOING:
        return false;
    }

    return false;
}

void BattleManager::LogDrawResult() {
    for (Unit *unit : allUnits) {
        unit->GetWeapon()->recordDraw();
        unit->GetArmor()->recordDraw();
    }
}

void BattleManager::LogWinLoseResult(BATTLERESULT result) {

    Team *winnerTeam;
    Team *loserTeam;

    if (result == REDWIN) {
        winnerTeam = battlefield.GetRedTeam();
        loserTeam = battlefield.GetBlueTeam();
    } else {
        winnerTeam = battlefield.GetBlueTeam();
        loserTeam = battlefield.GetRedTeam();
    }

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

void BattleManager::ProcessAfterActions(const std::vector<Unit *> &allUnits) {
    // Move all after-actions to be processed (clear the vector)
    std::vector<AfterActionEvent> toProcess = std::move(GlobalAction::afterActions);

    // Process each after-action
    for (const auto &afterActionEvent : toProcess) {
        Unit *actor = afterActionEvent.context.actor;

        std::vector<Unit *> targets;
        targets.reserve(5);

        // if boon then apply to the actor
        BoonAction *boonAction = dynamic_cast<BoonAction *>(afterActionEvent.battleAction);
        if (boonAction != nullptr) {
            targets = {afterActionEvent.context.target};
            LogSystem::LogStream("[AFTER-ACTION] ", actor->GetName(), " → ", afterActionEvent.context.target->GetName(),
                                 ": ", boonAction->GetID());
        }
        // otherwise just normal targetting
        else {
            targets = TargetManager::GetTargets(*actor, *afterActionEvent.battleAction);
        }

        if (targets.size() > 0) {
            for (Unit *target : targets) {

                afterActionEvent.battleAction->Perform(actor, target);
            }
        } else {
            LogSystem::LogStream("[AFTER-ACTION] No valid target found for ", actor->GetName());
        }
    }
    GlobalAction::afterActions.clear();
}

// Apply unit boons to after-action system
void BattleManager::ApplyUnitBoonsToAfterAction(Unit *unit) {
    if (unit->IsAlive()) {
        // The unit applies its boons to the after-action system
        unit->ApplyBoonsToAfterAction();
    }
}
