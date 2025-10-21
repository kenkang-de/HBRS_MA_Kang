#include <numeric>
#include <iostream>

#include "TurnManager.h"
#include "../Log/LogSystem.h"

int TurnManager::GCD(int a, int b) {
    return b == 0 ? a : GCD(b, a % b);
}

int TurnManager::ComputeLCM(const std::vector<int>& speeds) {
    if (speeds.empty()) return 1;
    
    int result = speeds[0];
    const int MAX_LCM = 100000; // Cap to prevent computational explosion
    
    for (size_t i = 1; i < speeds.size(); ++i) {
        if (speeds[i] <= 0) continue; // Skip invalid speeds
        
        int gcd = GCD(result, speeds[i]);
        
        // Check for potential overflow before computing
        if (result > MAX_LCM / speeds[i] * gcd) {
            return MAX_LCM; // Return capped value to prevent overflow
        }
        
        result = result * speeds[i] / gcd;
        
        // Cap the result to prevent runaway growth
        if (result > MAX_LCM) {
            return MAX_LCM;
        }
    }
    return result;
}

void TurnManager::Initialize(const std::vector<Unit*>& units) {
    std::vector<int> speeds;
    for (Unit* u : units) {
        int speed = u->GetTotalStat().GetSpeed();
        // Only include units with positive speed in calculations, and cap max speed to prevent LCM explosion
        if (speed > 0 && speed <= 1000) { // Cap individual speeds
            speeds.push_back(speed);
        }
    }

    // If no units have positive speed, set LCM to 1
    lcm = speeds.empty() ? 1 : ComputeLCM(speeds);
    tick = 0;

    while (!turnQueue.empty()) turnQueue.pop();
    lastActionTick.clear(); // CRITICAL: Clear previous battle data

    // Only schedule units with positive speed
    for (Unit* u : units) {
        int speed = u->GetTotalStat().GetSpeed();
        if (speed > 0) {
            int interval = lcm / speed;
            u->Tickinterval = interval;
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
    
    // Always rebuild when LCM changes - this will remove speed 0 units
    if (newLCM != lcm) {
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
        
        // ADD SPEED CHECK to prevent division by zero
        if (action.unit != nullptr && action.unit->IsAlive() && action.unit->GetTotalStat().GetSpeed() > 0) {
            int currentSpeed = action.unit->GetTotalStat().GetSpeed();
            
            // Double-check speed is still positive before division
            if (currentSpeed > 0) {
                int oldInterval = oldLCM / currentSpeed;
                int newInterval = lcm / currentSpeed;
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
            // If speed became 0, skip this unit (don't reschedule)
        }
    }
    
    // Add any units that weren't previously scheduled but are alive AND have positive speed
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
                int speed = unit->GetTotalStat().GetSpeed();
                if (speed > 0) {  // Double-check before division
                    int newInterval = lcm / speed;
                    newSchedule.push_back({unit, tick + newInterval});
                }
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

int TurnManager::GetUnitInterval(Unit* unit){
    return lcm/unit->GetTotalStat().GetSpeed();
}

std::vector<Unit*> TurnManager::GetNextUnits() {

    std::vector<Unit*> unitsToAct;

    if (turnQueue.empty()) 
    return unitsToAct;

    // SAFETY CHECK: If queue gets too large, something is wrong - force cleanup
    if (turnQueue.size() > 1000) {
        Reset(); // Nuclear option: reset everything to prevent runaway growth
        return unitsToAct;
    }

    //Clean up queue that might have actions for dead units or past ticks
    while (!turnQueue.empty()) {
        const ScheduledAction& top = turnQueue.top();
        // Remove if: past tick, dead unit, or zero speed unit
        if (top.nextTick + (top.unit ? top.unit->TickDelay : 0) < tick || 
            top.unit == nullptr || 
            !top.unit->IsAlive() || 
            top.unit->GetTotalStat().GetSpeed() <= 0) {
            
            // Clean up tracking for removed units
            if (top.unit != nullptr && (!top.unit->IsAlive() || top.unit->GetTotalStat().GetSpeed() <= 0)) {
                lastActionTick.erase(top.unit);
            }
            
            turnQueue.pop();
        } else {
            break; // Found valid action, stop cleaning
        }
    }

    // Collect all units scheduled for the current tick
    while (!turnQueue.empty() && turnQueue.top().nextTick + turnQueue.top().unit->TickDelay == tick) {
        ScheduledAction top = turnQueue.top();
        turnQueue.pop();
        
        // Only process and reschedule ALIVE units with positive speed
        if (top.unit != nullptr && top.unit->IsAlive() && top.unit->GetTotalStat().GetSpeed() > 0) {
            unitsToAct.push_back(top.unit);
            // Record when this unit acted
            lastActionTick[top.unit] = tick;

            int interval = GetUnitInterval(top.unit);
            // Reset TickDelay when rescheduling
            top.unit->TickDelay = 0;
            int nextTick = tick + interval;
            turnQueue.push({top.unit, nextTick});
        } else {
            // Dead unit or zero speed - remove from tracking
            if (top.unit != nullptr) {
                lastActionTick.erase(top.unit);
            }
        }
    }

    return unitsToAct;
}

void TurnManager::DelayUnit(Unit* targetUnit, int delayTicks) {
    if (targetUnit == nullptr || delayTicks <= 0) return;

    targetUnit->TickDelay += delayTicks;  
}

void TurnManager::ResetMagicUnitTick(Unit* magicUnit) {
    if (magicUnit == nullptr || !magicUnit->IsAlive()) return;
    
    int unitSpeed = magicUnit->GetTotalStat().GetSpeed();
    
    // Don't reset units with speed <= 0, just remove them from queue
    if (unitSpeed <= 0) {
        // Create temporary storage for all scheduled actions
        std::vector<ScheduledAction> allActions;
        
        // Extract all actions from the queue
        while (!turnQueue.empty()) {
            allActions.push_back(turnQueue.top());
            turnQueue.pop();
        }
        
        // Rebuild the queue without the speed-0 unit
        for (const auto& action : allActions) {
            if (action.unit != magicUnit && action.unit != nullptr && action.unit->IsAlive() && action.unit->GetTotalStat().GetSpeed() > 0) {
                turnQueue.push(action);
            }
        }
        
        return;
    }
    
    // Create temporary storage for all scheduled actions
    std::vector<ScheduledAction> allActions;
    
    // Extract all actions from the queue
    while (!turnQueue.empty()) {
        allActions.push_back(turnQueue.top());
        turnQueue.pop();
    }
    
    // Find and reset the magic unit's next action tick
    for (auto& action : allActions) {
        if (action.unit == magicUnit && magicUnit->IsAlive()) {
            // FIXED: Use proper LCM-based interval instead of raw speed
            int interval = lcm / unitSpeed;
            action.nextTick = tick + interval;
            break; // Only reset the first (next) occurrence of this unit
        }
    }
    
    // Rebuild the queue with the modified actions (only alive units with positive speed)
    for (const auto& action : allActions) {
        if (action.unit != nullptr && action.unit->IsAlive() && action.unit->GetTotalStat().GetSpeed() > 0) {
            turnQueue.push(action);
        }
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
    for (const auto& action : aliveActions) {
        turnQueue.push(action);
    }
}

void TurnManager::Reset() {
    tick = 0;
    lcm = 1;
    while (!turnQueue.empty()) {
        turnQueue.pop();
    }
    lastActionTick.clear();
}