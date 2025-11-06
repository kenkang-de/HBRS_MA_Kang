#ifndef TURNMAN_H
#define TURNMAN_H

#include "Unit.h"
#include <algorithm>
#include <queue>
#include <vector>

struct ScheduledAction {
    Unit *unit;
    int nextTick;

    bool operator>(const ScheduledAction &other) const {
        return nextTick > other.nextTick;
    }
};

class TurnManager {
  public:
    void Initialize(std::vector<Unit *> &units);
    std::vector<Unit *> GetNextUnits();
    void AdvanceTick();
    bool CanContinue(std::vector<Unit *> units);
    int GetCurrentTick() const {
        return tick;
    }
    static void UpdateSpeedChanges();
    void DelayUnit(Unit *unit, int delayTicks); // Add delay to a specific unit
    void ResetMagicUnitTick(Unit *magicUnit);   // Reset magic unit's next action tick
    void RemoveDeadUnits(const std::vector<Unit *> &units);

    void RevalidateEntry(std::vector<Unit *> &units);

    static std::vector<Unit *> *allUnitsPtr;

    static int GreatestCommonDivisor(int a, int b);
    static int ComputeLCM(std::vector<Unit *> units);
    static int LCM;

  private:
    int tick = 1;

    // The lowest value is always at the top,
    std::priority_queue<ScheduledAction, std::vector<ScheduledAction>, std::greater<>> turnQueue;
};

#endif
