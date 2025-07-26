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
    Unit* GetNextUnit();
    void AdvanceTick();
    bool HasActions() const;

private:
    int tick = 0;
    int lcm = 1;
    std::priority_queue<ScheduledAction, std::vector<ScheduledAction>, std::greater<>> turnQueue;

    int ComputeLCM(const std::vector<int>& speeds);
    int GCD(int a, int b);
};

#endif
