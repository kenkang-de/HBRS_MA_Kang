#include "TurnManager.h"
#include <numeric>
#include <iostream>

int TurnManager::GCD(int a, int b) {
    return b == 0 ? a : GCD(b, a % b);
}

int TurnManager::ComputeLCM(const std::vector<int>& speeds) {
    int result = speeds[0];
    for (size_t i = 1; i < speeds.size(); ++i) {
        result = result * speeds[i] / GCD(result, speeds[i]);
    }
    return result;
}

void TurnManager::Initialize(const std::vector<Unit*>& units) {
    std::vector<int> speeds;
    for (Unit* u : units) {
        speeds.push_back(u->GetTotalStat().GetSpeed());
    }

    lcm = ComputeLCM(speeds);
    tick = 0;

    while (!turnQueue.empty()) turnQueue.pop();

    for (Unit* u : units) {
        int interval = lcm / u->GetTotalStat().GetSpeed();
        turnQueue.push({u, interval});
    }
}

Unit* TurnManager::GetNextUnit() {
    if (turnQueue.empty()) return nullptr;

    ScheduledAction top = turnQueue.top();
    if (top.nextTick > tick) return nullptr;

    turnQueue.pop();

    int interval = lcm / top.unit->GetTotalStat().GetSpeed();
    turnQueue.push({top.unit, tick + interval});

    // std::cout << "[TurnManager] Next unit speed: "
            //   << top.unit->GetTotalStat().GetSpeed() << "\n";

    return top.unit;
}

void TurnManager::AdvanceTick() {
    ++tick;
}

bool TurnManager::HasActions() const {
    return !turnQueue.empty();
}
