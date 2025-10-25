#include "BattleAction.h"
#include "Unit.h"
#include <iostream>

const std::string &BattleAction::GetID() const {
    return ID;
}

void BattleAction::AddConditionalAction(const std::string &conditionID, const std::string &actionID,
                                        const std::string &param) {
    conditionalActions.emplace_back(ActionLibrary::GetCondition(conditionID),
                                    param.empty() ? ActionLibrary::GetAction(actionID)
                                                  : ActionLibrary::GetAction(actionID, param));
}

void BattleAction::Perform(Unit *actor, Unit *target, const std::vector<Unit *> &allies,
                           const std::vector<Unit *> &enemies) const {
    if (target) {
        ActionContext ctx{actor, target, allies, enemies};

        if (conditionalActions.empty()) {
            return;
        }

        bool actionExecuted = false;
        for (const auto &[condition, action] : conditionalActions) {
            if (condition(ctx)) {
                action(ctx);
                actionExecuted = true;
            }
        }
    }
}

int BattleAction::CalculateDamage(Unit *actingUnit, Unit *target) const {
    // Implement damage calculation logic here
    return actingUnit->GetTotalStat().GetAttack() - target->GetTotalStat().GetDefense();
}
