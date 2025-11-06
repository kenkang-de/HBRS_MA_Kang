#include <iostream>
#include <numeric>

#include "../Log/LogSystem.h"
#include "TurnManager.h"

int TurnManager::LCM = 1;
std::vector<Unit *> *TurnManager::allUnitsPtr = nullptr;

int TurnManager::GreatestCommonDivisor(int a, int b) {
    return b == 0 ? a : GreatestCommonDivisor(b, a % b);
}

int TurnManager::ComputeLCM(std::vector<Unit *> units) {

    int result = 1;
    for (Unit *unit : units) {
        if (unit != nullptr) {
            int speed = unit->GetTotalStat().GetSpeed();
            if (speed <= 0)
                continue;
            int gcd = GreatestCommonDivisor(result, speed);
            result = result * speed / gcd;
        }
    }
    return result;
}

void TurnManager::Initialize(std::vector<Unit *> &units) {

    allUnitsPtr = &units;

    tick = 0;

    while (!turnQueue.empty())
        turnQueue.pop();

    LCM = ComputeLCM(units);

    for (Unit *unit : units) {
        int speed = unit->GetTotalStat().GetSpeed();
        unit->Tickinterval = speed > 0 ? LCM / speed : LCM;
        turnQueue.push({unit, unit->Tickinterval});
    }
}

void TurnManager::UpdateSpeedChanges() {

    LCM = ComputeLCM(*allUnitsPtr);

    for (Unit *unit : *allUnitsPtr) {
        if (unit->IsAlive() && unit->GetTotalStat().GetSpeed() > 0) {
            int speed = unit->GetTotalStat().GetSpeed();
            unit->Tickinterval = speed > 0 ? LCM / speed : LCM;
        }
    }
}

void TurnManager::AdvanceTick() {
    ++tick;
}

bool TurnManager::CanContinue(std::vector<Unit *> units) {
    for (Unit *unit : units) {
        if (unit->IsAlive() && unit->GetTotalStat().GetSpeed() > 0) {
            return true;
        }
    }
    return false;
}

// TODO: investigate isfrozen
std::vector<Unit *> TurnManager::GetNextUnits() {

    std::vector<Unit *> unitsToAct;

    // if there is no units left to act at the battle
    if (turnQueue.empty())
        return unitsToAct;

    if (tick < turnQueue.top().nextTick + turnQueue.top().unit->TickDelay)
        tick = turnQueue.top().nextTick + turnQueue.top().unit->TickDelay;

    // Pop out dead unit
    while (!turnQueue.empty()) {
        const ScheduledAction &top = turnQueue.top();
        if (top.unit == nullptr)
            std::cerr << "Unit pointer null at (GetNextUnits())" << std::endl;
        else if (!top.unit->IsAlive())
            turnQueue.pop();
        // Found valid action, stop cleaning
        else
            break;
    }

    // If all dead
    if (turnQueue.empty())
        return unitsToAct;

    // add to unitsToAct (return vector Unit*) units that is at it's turn.
    while (turnQueue.top().nextTick + turnQueue.top().unit->TickDelay <= tick) {

        int nextTick;

        while (turnQueue.top().nextTick + turnQueue.top().unit->TickDelay < tick) {
            nextTick = turnQueue.top().nextTick + turnQueue.top().unit->Tickinterval;
            turnQueue.push({turnQueue.top().unit, nextTick});
            turnQueue.pop();
        }

        if (turnQueue.empty())
            return unitsToAct;

        if (turnQueue.top().unit->GetTotalStat().GetSpeed() > 0 && !turnQueue.top().unit->IsFrozen())
            unitsToAct.push_back(turnQueue.top().unit);

        // Reset Tickdelay after an act
        turnQueue.top().unit->TickDelay = 0;
        nextTick = tick + turnQueue.top().unit->Tickinterval;
        // Reschedule turnQueue
        turnQueue.push({turnQueue.top().unit, nextTick});
        turnQueue.pop();
    }

    return unitsToAct;
}

void TurnManager::DelayUnit(Unit *targetUnit, int delayTicks) {
    targetUnit->TickDelay += std::max(delayTicks, 0);
}

void TurnManager::ResetMagicUnitTick(Unit *magicUnit) {

    magicUnit->TickDelay = magicUnit->Tickinterval;
}

void TurnManager::RemoveDeadUnits(const std::vector<Unit *> &units) {
    std::vector<ScheduledAction> aliveActions;

    while (!turnQueue.empty()) {
        ScheduledAction action = turnQueue.top();
        turnQueue.pop();

        // Only keep actions for alive units
        if (action.unit != nullptr && action.unit->IsAlive()) {
            aliveActions.push_back(action);
        }
    }

    // Rebuild queue with only alive units
    for (const auto &action : aliveActions) {
        turnQueue.push(action);
    }
}

// make sure that units stay in the respective tick.
// this prevents modified units during battle not to act. (with repect to Range/ Melee/ Magic relationship)
void TurnManager::RevalidateEntry(std::vector<Unit *> &units) {
    units.erase(std::remove_if(units.begin(), units.end(),
                               [](Unit *unit) {
                                   return !unit->IsAlive() || unit->TickDelay > 0 || unit->IsFrozen() ||
                                          unit->GetTotalStat().GetSpeed() <= 0;
                               }),
                units.end());
}