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
    // Check if any alive unit's speed has actually changed
    std::vector<int> currentSpeeds;
    for (Unit* u : units) {
        if (u != nullptr && u->IsAlive() && u->GetTotalStat().GetSpeed() > 0) {
            currentSpeeds.push_back(u->GetTotalStat().GetSpeed());
        }
    }
    
    if (currentSpeeds.empty()) return;
    
    int newLCM = ComputeLCM(currentSpeeds);
    
    // Only rebuild if LCM actually changed
    if (newLCM != lcm) {
        std::cout << "[DEBUG] LCM changed from " << lcm << " to " << newLCM << " - rebuilding queue" << std::endl;
        lcm = newLCM;
        RebuildQueue(units);
    }
}

void TurnManager::RebuildQueue(const std::vector<Unit*>& units) {
    // Store the old LCM for scaling calculations
    int oldLCM = lcm;
    
    // Store units and their new calculated times
    std::vector<ScheduledAction> newSchedule;
    
    // Process existing scheduled actions and calculate new times
    while (!turnQueue.empty()) {
        ScheduledAction action = turnQueue.top();
        turnQueue.pop();
        
        if (action.unit != nullptr && action.unit->IsAlive() && action.unit->GetTotalStat().GetSpeed() > 0) {
            int oldInterval = oldLCM / action.unit->GetTotalStat().GetSpeed();
            int newInterval = lcm / action.unit->GetTotalStat().GetSpeed();
            int plannedTick = action.nextTick;
            int ticksRemaining = plannedTick - tick;
            
            int newNextTick;
            if (ticksRemaining > 0) {
                // Scale the remaining time by LCM ratio
                double lcmScale = (double)lcm / oldLCM;
                int scaledRemaining = (int)(ticksRemaining * lcmScale);
                newNextTick = tick + scaledRemaining;
            } else {
                // Unit was scheduled for current or past tick, use new interval
                newNextTick = tick + newInterval;
            }
            
            newSchedule.push_back({action.unit, newNextTick});
        }
    }
    
    // Add any units that weren't previously scheduled but are alive
    for (Unit* unit : units) {
        if (unit && unit->IsAlive() && unit->GetTotalStat().GetSpeed() > 0) {
            // Check if this unit was already processed
            bool alreadyScheduled = false;
            for (const auto& scheduled : newSchedule) {
                if (scheduled.unit == unit) {
                    alreadyScheduled = true;
                    break;
                }
            }
            
            if (!alreadyScheduled) {
                int newInterval = lcm / unit->GetTotalStat().GetSpeed();
                newSchedule.push_back({unit, tick + newInterval});
            }
        }
    }
    
    // Add all calculated schedules to the queue
    for (const auto& scheduled : newSchedule) {
        turnQueue.push(scheduled);
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
        
        // Only add alive units to the action list
        if (top.unit != nullptr && top.unit->IsAlive()) {
            unitsToAct.push_back(top.unit);
            // Record when this unit acted
            lastActionTick[top.unit] = tick;
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

void TurnManager::ResetMagicUnitTick(Unit* magicUnit) {
    if (magicUnit == nullptr) return;
    
    // Create temporary storage for all scheduled actions
    std::vector<ScheduledAction> allActions;
    
    // Extract all actions from the queue
    while (!turnQueue.empty()) {
        allActions.push_back(turnQueue.top());
        turnQueue.pop();
    }
    
    // Find and reset the magic unit's next action tick
    for (auto& action : allActions) {
        if (action.unit == magicUnit) {
            // Reset to current tick + unit's speed
            int speed = magicUnit->GetTotalStat().GetSpeed();
            if (speed > 0) {
                action.nextTick = tick + speed;
            }
            break; // Only reset the first (next) occurrence of this unit
        }
    }
    
    // Rebuild the queue with the modified actions
    for (const auto& action : allActions) {
        turnQueue.push(action);
    }
}

void TurnManager::RemoveDeadUnits(const std::vector<Unit*>& units) {
    // Clean up lastActionTick for dead units
    auto it = lastActionTick.begin();
    while (it != lastActionTick.end()) {
        if (!it->first->IsAlive()) {
            it = lastActionTick.erase(it);
        } else {
            ++it;
        }
    }
}
