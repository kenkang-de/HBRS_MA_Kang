#include "BoonAction.h"
#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "Unit.h"

void BoonAction::Perform(Unit *actor, Unit *target) {
    if (target) {
        ActionContext ctx;

        if (this->Caster != nullptr)
            ctx = ActionContext{this->Caster, target};
        else
            ctx = ActionContext{actor, target};

        LogSystem::LogStream("[BOON] ", ID, " on ", target->GetName(), " (Usage: ", usageNumber, "/", maxUsage, ")");

        // Execute all conditional actions for this boon
        if (!conditionalActions.empty()) {
            for (const auto &[condition, action] : conditionalActions) {
                if (condition(ctx)) {
                    action(ctx);
                }
            }
        }
        const_cast<BoonAction *>(this)->DecrementUsage();

        if (IsExpired()) {
            target->CleanupExpiredBoons();
        }
    }
}
