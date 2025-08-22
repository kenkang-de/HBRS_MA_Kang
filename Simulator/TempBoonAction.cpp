#include "TempBoonAction.h"
#include "Unit.h"
#include "ActionLibrary.h"
#include <iostream>

TempBoonAction::TempBoonAction() : BoonAction(), hasBeenApplied(false), effectExecuted(false), removalEffectName("") {}

TempBoonAction::TempBoonAction(const std::string& id, const std::string& effectType, int duration, const std::string& removalEffect)
    : BoonAction(id, effectType, duration), hasBeenApplied(false), effectExecuted(false), removalEffectName(removalEffect) {
}

void TempBoonAction::Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const {
    if (target) {
        ActionContext ctx{ actor, target, allies, enemies };

        if (!effectExecuted) {
            // Apply the buff effect only once
            std::cout << "[TEMP-BUFF] " << GetEffectType() << " applied to " << target->GetName() 
                      << " (Duration: " << GetUsageNumber() << " turns)" << std::endl;

            // Execute all conditional actions for this buff
            if (!conditionalActions.empty()) {
                for (const auto& [condition, action] : conditionalActions) {
                    if (condition(ctx)) {
                        action(ctx);
                    }
                }
            }
            
            // Mark as applied and effect executed
            MarkAsApplied();
            MarkEffectExecuted();
        } else {
            // Count down duration and decrement usage for subsequent turns
            std::cout << "[TEMP-BUFF] " << GetEffectType() << " on " << target->GetName() 
                      << " (Remaining: " << (GetUsageNumber() - 1) << " turns)" << std::endl;
            
            // Only decrement usage after the first application
            const_cast<TempBoonAction*>(this)->DecrementUsage();
        }
        
        // Handle expiration (only check after potential decrement)
        if (effectExecuted && IsExpired()) {
            std::cout << "[TEMP-BUFF] " << GetEffectType() << " expired on " << target->GetName();
            
            // Apply removal effect if specified
            if (!removalEffectName.empty()) {
                std::cout << " - applying removal effect: " << removalEffectName << std::endl;
                
                // Use EXECUTE_EFFECT to apply removal BattleAction
                ActionFn removalAction = ActionLibrary::GetAction("EXECUTE_EFFECT", removalEffectName);
                ActionContext removalContext = {actor, target, allies, enemies};
                removalAction(removalContext);
            } else {
                std::cout << " - no removal effect specified" << std::endl;
            }
            
            // Clean up expired boons
            actor->CleanupExpiredBoons();
        }
    }
}
