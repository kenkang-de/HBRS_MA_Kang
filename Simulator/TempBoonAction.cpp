#include "TempBoonAction.h"
#include "Unit.h"
#include "ActionLibrary.h"
#include <iostream>

TempBoonAction::TempBoonAction() : BoonAction(), hasBeenApplied(false), removalEffectName("") {}

TempBoonAction::TempBoonAction(const std::string& id, const std::string& effectType, int duration, const std::string& removalEffect)
    : BoonAction(id, effectType, duration), hasBeenApplied(false), removalEffectName(removalEffect) {
}

void TempBoonAction::Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const {
    if (target) {
        ActionContext ctx{ actor, target, allies, enemies };

        if (!hasBeenApplied) {
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
            
            // Mark as applied so it doesn't apply again
            MarkAsApplied();
        } else {
            // Just count down duration without triggering effects
            std::cout << "[TEMP-BUFF] " << GetEffectType() << " on " << target->GetName() 
                      << " (Remaining: " << (GetUsageNumber() - 1) << " turns)" << std::endl;
        }
        
        // Decrement usage after performing
        const_cast<TempBoonAction*>(this)->DecrementUsage();
        
        // Handle expiration
        if (IsExpired()) {
            std::cout << "[TEMP-BUFF] " << GetEffectType() << " expired on " << target->GetName();
            
            // Apply removal effect if specified
            if (!removalEffectName.empty()) {
                const BattleAction* removalAction = ActionLibrary::GetGlobalAction(removalEffectName);
                if (removalAction) {
                    std::cout << " - applying removal effect: " << removalEffectName << std::endl;
                    removalAction->Perform(actor, target, allies, enemies);
                } else {
                    std::cout << " - removal effect '" << removalEffectName << "' not found!" << std::endl;
                }
            } else {
                std::cout << " - no removal effect specified" << std::endl;
            }
            
            // Clean up expired boons
            actor->CleanupExpiredBoons();
        }
    }
}
