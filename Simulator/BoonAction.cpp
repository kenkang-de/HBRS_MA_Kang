#include "BoonAction.h"
#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "Unit.h"

BoonAction::BoonAction() : BattleAction(), usageNumber(0), maxUsage(0), effectType("") {}

BoonAction::BoonAction(const std::string &id, const std::string &effectType, int usage)
    : BattleAction(), effectType(effectType), usageNumber(usage), maxUsage(usage) {
    SetID(id);
}

void BoonAction::Perform(Unit *actor, Unit *target) {
    if (target) {
        ActionContext ctx{actor, target};

        LogSystem::LogStream("[BOON] ", effectType, " triggered on ", target->GetName(), " (Usage: ", usageNumber, "/",
                             maxUsage, ")");

        // Execute all conditional actions for this boon
        if (!conditionalActions.empty()) {
            for (const auto &[condition, action] : conditionalActions) {
                if (condition(ctx)) {
                    action(ctx);
                }
            }
        }

        // Decrement usage after performing
        const_cast<BoonAction *>(this)->DecrementUsage();

        // Clean up expired boons on the actor (the unit that has this boon) after this boon performs
        if (IsExpired()) {
            actor->CleanupExpiredBoons();
        }
    }
}
