#ifndef TURNMAN_H
#define TURNMAN_H

#include "Unit.h"
#include <vector>
#include <queue>

struct ScheduledAction {
    Unit* unit;
    int nextTick;

    bool operator>(const ScheduledAction& other) const {
        return nextTick > other.nextTick; // min-heap
    }
};

class TurnManager {
public:
    void Initialize(const std::vector<Unit*>& units);
    std::vector<Unit*> GetNextUnits(); 
    void AdvanceTick();
    bool HasActions() const;
    int GetCurrentTick() const { return tick; }
    void UpdateSpeedChanges(const std::vector<Unit*>& units); // New method for dynamic updates
    void DelayUnit(Unit* unit, int delayTicks); // Add delay to a specific unit
    void ResetMagicUnitTick(Unit* magicUnit); // Reset magic unit's next action tick

private:
    int tick = 0;
    int lcm = 1;
    std::priority_queue<ScheduledAction, std::vector<ScheduledAction>, std::greater<>> turnQueue;

    int ComputeLCM(const std::vector<int>& speeds);
    int GCD(int a, int b);
    void RebuildQueue(const std::vector<Unit*>& units); // Helper method to rebuild queue
};

#endif
