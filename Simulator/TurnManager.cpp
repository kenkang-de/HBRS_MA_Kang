#include <iostream>
#include <numeric>

#include "../Log/LogSystem.h"
#include "TurnManager.h"

int TurnManager::LCM = 1;
std::vector<Unit *> *TurnManager::allUnitsPtr = nullptr;
std::multimap<int, Unit *> TurnManager::turnMap;

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

    turnMap.clear();

    LCM = ComputeLCM(units);

    for (Unit *unit : units) {
        int speed = unit->GetTotalStat().GetSpeed();
        unit->Tickinterval = speed > 0 ? LCM / speed : LCM;
        turnMap.insert({unit->Tickinterval, unit});
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

bool TurnManager::CanContinue(std::vector<Unit *> units) {
    for (Unit *unit : units) {
        if (unit->IsAlive() && unit->GetTotalStat().GetSpeed() > 0) {
            return true;
        }
    }
    return false;
}

void TurnManager::ApplyDelays() {
    std::vector<std::multimap<int, Unit *>::iterator> toRemove;

    for (auto it = turnMap.begin(); it != turnMap.end(); ++it) {
        Unit *unit = it->second;

        if (unit->TickDelay > 0) {
            turnMap.insert({it->first + unit->TickDelay, unit});
            unit->TickDelay = 0;
            toRemove.push_back(it);
        }
    }

    for (auto it : toRemove) {
        turnMap.erase(it);
    }
}

// TODO: investigate isfrozen
std::vector<Unit *> TurnManager::GetNextUnits() {

    std::vector<Unit *> unitsToAct;

    RemoveDeadUnitsFromMap();

    // annihilated
    if (turnMap.empty())
        return unitsToAct;

    // get earliest tick (next tick + delay)
    tick = turnMap.begin()->first + turnMap.begin()->second->TickDelay;

    std::vector<std::multimap<int, Unit *>::iterator> toRemove;

    for (auto it = turnMap.begin(); it != turnMap.end(); ++it) {
        Unit *unit = it->second;
        int scheduledTick = it->first;
        int actualTick = scheduledTick + unit->TickDelay;

        if (actualTick == tick) {
            if (unit->GetTotalStat().GetSpeed() > 0 && !unit->IsFrozen()) {
                unitsToAct.push_back(unit);
            }
            unit->TickDelay = 0;
            int nextActionTick = tick + unit->Tickinterval;
            turnMap.insert({nextActionTick, unit});
            toRemove.push_back(it);
        }
    }

    for (auto it : toRemove) {
        turnMap.erase(it);
    }

    return unitsToAct;
}

void TurnManager::ResetMagicUnitTick(Unit *magicUnit) {

    magicUnit->TickDelay = magicUnit->Tickinterval;
}

void TurnManager::RemoveDeadUnitsFromMap() {
    auto it = turnMap.begin();
    while (it != turnMap.end()) {
        if (!it->second->IsAlive()) {
            it = turnMap.erase(it);
        } else {
            ++it;
        }
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