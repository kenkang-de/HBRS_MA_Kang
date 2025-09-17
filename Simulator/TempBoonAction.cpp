#include <iostream>

#include "TempBoonAction.h"
#include "Unit.h"
#include "ActionLibrary.h"
#include "../Log/LogSystem.h"

TempBoonAction::TempBoonAction() : BoonAction(), hasBeenApplied(false), effectExecuted(false), removalEffectName("") {}

TempBoonAction::TempBoonAction(const std::string& id, const std::string& effectType, int duration, const std::string& removalEffect)
    : BoonAction(id, effectType, duration), hasBeenApplied(false), effectExecuted(false), removalEffectName(removalEffect) {
}

void TempBoonAction::Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const {
    if (target) {
        ActionContext ctx{ actor, target, allies, enemies };

        if (!effectExecuted) {
            // Apply the buff effect only once
            LogSystem::LogStream( "[TEMP-BUFF] " , GetEffectType() , " applied to " , target->GetName() 
                      , " (Duration: " , GetUsageNumber() , " turns)" );

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
            LogSystem::LogStream( "[TEMP-BUFF] " , GetEffectType() , " on " , target->GetName() 
                      , " (Remaining: " , (GetUsageNumber() - 1) , " turns)" );
            
            // Only decrement usage after the first application
            const_cast<TempBoonAction*>(this)->DecrementUsage();
        }
        
        // Handle expiration (only check after potential decrement)
        if (effectExecuted && IsExpired()) {
            LogSystem::LogStream( "[TEMP-BUFF] " , GetEffectType() , " expired on " , target->GetName());
            
            // Apply removal effect if specified
            if (!removalEffectName.empty()) {
                LogSystem::LogStream( " - applying removal effect: " , removalEffectName );
                
                // Use EXECUTE_EFFECT to apply removal BattleAction
                ActionFn removalAction = ActionLibrary::GetAction("EXECUTE_EFFECT", removalEffectName);
                ActionContext removalContext = {actor, target, allies, enemies};
                removalAction(removalContext);
            } else {
                LogSystem::LogStream( " - no removal effect specified" );
            }
            
            // Clean up expired boons
            actor->CleanupExpiredBoons();
        }
    }
}
