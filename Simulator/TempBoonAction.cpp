#include <iostream>

#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "TempBoonAction.h"
#include "Unit.h"

TempBoonAction::TempBoonAction(std::string id, std::string effectType, int duration, std::string removalEffect,
                               Unit *caster)
    : BoonAction(id, effectType, duration, caster), removalEffectName(removalEffect) {}

void TempBoonAction::Perform(Unit *actor, Unit *target) {
    if (target) {

        ActionContext ctx;

        if (this->Caster != nullptr)
            ctx = ActionContext{this->Caster, target};
        else
            ctx = ActionContext{actor, target};

        // Apply the buff effect only once
        if (!effectExecuted) {

            // Execute all conditional actions for this buff
            if (!conditionalActions.empty()) {
                for (const auto &[condition, action] : conditionalActions) {
                    if (condition(ctx)) {
                        action(ctx);
                    }
                }
            }

            // Mark as applied and effect executed
            MarkAsApplied();
            MarkEffectExecuted();
        } else {

            // Only decrement usage after the first application
            const_cast<TempBoonAction *>(this)->DecrementUsage();
        }

        // Handle expiration (only check after potential decrement)
        if (effectExecuted && IsExpired()) {
            LogSystem::LogStream("[TEMP-BUFF] ", GetEffectType(), " expired on ", target->GetName());

            // Apply removal effect if specified
            if (!removalEffectName.empty()) {
                LogSystem::LogStream(" - applying removal effect: ", removalEffectName);

                // Use EXECUTE_EFFECT to apply removal BattleAction
                ActionFn removalAction = ActionLibrary::GetAction("EXECUTE_EFFECT", removalEffectName);
                ActionContext removalContext = {ctx.actor, ctx.target};
                removalAction(removalContext);
            } else {
                LogSystem::LogStream(" - no removal effect specified");
            }
            target->CleanupExpiredBoons();
        }
    }
}
