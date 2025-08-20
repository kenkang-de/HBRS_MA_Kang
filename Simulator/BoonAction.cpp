#include "BoonAction.h"
#include "Unit.h"
#include "ActionLibrary.h"
#include <iostream>

BoonAction::BoonAction() : BattleAction(), usageNumber(0), maxUsage(0), effectType("") {}

BoonAction::BoonAction(const std::string& id, const std::string& effectType, int usage)
    : BattleAction(), effectType(effectType), usageNumber(usage), maxUsage(usage) {
    SetID(id);
}

void BoonAction::Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const {
    if (target) {
        ActionContext ctx{ actor, target, allies, enemies };

        std::cout << "[BOON] " << effectType << " triggered on " << target->GetName() 
                  << " (Usage: " << usageNumber << "/" << maxUsage << ")" << std::endl;

        // Execute all conditional actions for this boon
        if (!conditionalActions.empty()) {
            for (const auto& [condition, action] : conditionalActions) {
                if (condition(ctx)) {
                    action(ctx);
                }
            }
        }
        
        // Decrement usage after performing
        const_cast<BoonAction*>(this)->DecrementUsage();
        
        // Clean up expired boons on the actor (the unit that has this boon) after this boon performs
        if (IsExpired()) {
            actor->CleanupExpiredBoons();
        }
    }
}
