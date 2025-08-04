#include "TurnManager.h"
#include <numeric>
#include <iostream>

int TurnManager::GCD(int a, int b) {
    return b == 0 ? a : GCD(b, a % b);
}

int TurnManager::ComputeLCM(const std::vector<int>& speeds) {
    if (speeds.empty()) return 1;
    
    int result = speeds[0];
    for (size_t i = 1; i < speeds.size(); ++i) {
        if (speeds[i] <= 0) continue; // Skip invalid speeds
        result = result * speeds[i] / GCD(result, speeds[i]);
    }
    return result;
}

void TurnManager::Initialize(const std::vector<Unit*>& units) {
    std::vector<int> speeds;
    for (Unit* u : units) {
        int speed = u->GetTotalStat().GetSpeed();
        // Only include units with positive speed in calculations
        if (speed > 0) {
            speeds.push_back(speed);
        }
    }

    // If no units have positive speed, set LCM to 1
    lcm = speeds.empty() ? 1 : ComputeLCM(speeds);
    tick = 0;

    while (!turnQueue.empty()) turnQueue.pop();

    // Only schedule units with positive speed
    for (Unit* u : units) {
        int speed = u->GetTotalStat().GetSpeed();
        if (speed > 0) {
            int interval = lcm / speed;
            turnQueue.push({u, interval});
        }
        // Units with speed <= 0 are not scheduled at all
    }
}

void TurnManager::UpdateSpeedChanges(const std::vector<Unit*>& units) {
    // Recalculate LCM with current speeds (only alive units)
    std::vector<int> currentSpeeds;
    for (Unit* u : units) {
        if (u != nullptr && u->IsAlive() && u->GetTotalStat().GetSpeed() > 0) {
            currentSpeeds.push_back(u->GetTotalStat().GetSpeed());
        }
    }
    
    if (currentSpeeds.empty()) return;
    
    int newLCM = ComputeLCM(currentSpeeds);
    
    // If LCM changed, rebuild the entire queue
    if (newLCM != lcm) {
        lcm = newLCM;
        RebuildQueue(units);
    }
}

void TurnManager::RebuildQueue(const std::vector<Unit*>& units) {
    // Store current units and their relative positions
    std::vector<std::pair<Unit*, int>> currentUnits;
    
    // Extract all units from the queue
    while (!turnQueue.empty()) {
        ScheduledAction action = turnQueue.top();
        turnQueue.pop();
        if (action.unit != nullptr) {
            // Calculate how many ticks until this unit's next action
            int ticksRemaining = action.nextTick - tick;
            currentUnits.push_back({action.unit, ticksRemaining});
        }
    }
    
    // Rebuild queue with new LCM
    for (const auto& [unit, ticksRemaining] : currentUnits) {
        // Only reschedule alive units with positive speed
        if (unit && unit->IsAlive() && unit->GetTotalStat().GetSpeed() > 0) {
            int newInterval = lcm / unit->GetTotalStat().GetSpeed();
            // Maintain relative timing as much as possible
            int newNextTick = tick + std::max(1, ticksRemaining * newInterval / (lcm / unit->GetTotalStat().GetSpeed()));
            turnQueue.push({unit, newNextTick});
        }
        // Units with speed <= 0 are not rescheduled
    }
}
void TurnManager::AdvanceTick() {
    ++tick;
}

bool TurnManager::HasActions() const {
    return !turnQueue.empty();
}

std::vector<Unit*> TurnManager::GetNextUnits() {
    std::vector<Unit*> unitsToAct;

    if (turnQueue.empty()) return unitsToAct;

    // Collect all units scheduled for the current tick
    while (!turnQueue.empty() && turnQueue.top().nextTick == tick) {
        ScheduledAction top = turnQueue.top();
        
        // Null pointer check
        if (top.unit != nullptr) {
            unitsToAct.push_back(top.unit);
        }
        
        turnQueue.pop();

        // Reschedule the unit if it's valid, alive, AND has positive speed
        if (top.unit != nullptr && top.unit->IsAlive() && top.unit->GetTotalStat().GetSpeed() > 0) {
            int interval = lcm / top.unit->GetTotalStat().GetSpeed();
            turnQueue.push({top.unit, tick + interval});
        }
        // Units with speed <= 0 are not rescheduled
    }

    return unitsToAct;
}

void TurnManager::DelayUnit(Unit* targetUnit, int delayTicks) {
    if (targetUnit == nullptr || delayTicks <= 0) return;
    
    // Create temporary storage for all scheduled actions
    std::vector<ScheduledAction> allActions;
    
    // Extract all actions from the queue
    while (!turnQueue.empty()) {
        allActions.push_back(turnQueue.top());
        turnQueue.pop();
    }
    
    // Find and delay the target unit's next action
    for (auto& action : allActions) {
        if (action.unit == targetUnit) {
            action.nextTick += delayTicks;
            break; // Only delay the first (next) occurrence of this unit
        }
    }
    
    // Rebuild the queue with the modified actions
    for (const auto& action : allActions) {
        turnQueue.push(action);
    }
}
    