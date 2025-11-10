#ifndef TURNMAN_H
#define TURNMAN_H

#include "Unit.h"
#include <algorithm>
#include <queue>
#include <vector>

class TurnManager {
  public:
    void Initialize(std::vector<Unit *> &units);
    std::vector<Unit *> GetNextUnits();
    bool CanContinue(std::vector<Unit *> units);
    int GetCurrentTick() const {
        return tick;
    }
    static void UpdateSpeedChanges();
    static void ApplyDelays();
    void ResetMagicUnitTick(Unit *magicUnit); // Reset magic unit's next action tick
    void RemoveDeadUnitsFromMap();

    void RevalidateEntry(std::vector<Unit *> &units);

    static std::vector<Unit *> *allUnitsPtr;

    static int GreatestCommonDivisor(int a, int b);
    static int ComputeLCM(std::vector<Unit *> units);
    static int LCM;

  private:
    int tick = 1;

    static std::multimap<int, Unit *> turnMap;
};

#endif
