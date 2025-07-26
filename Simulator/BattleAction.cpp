#include "BattleAction.h"
#include "TargetManager.h"


const std::string& BattleAction::GetID() const {
    return ID;
}

void BattleAction::AddConditionalAction(const std::string& conditionID, const std::string& actionID, const std::string& param) {
    conditionalActions.emplace_back(
        ActionLibrary::GetCondition(conditionID),
        ActionLibrary::GetAction(actionID, param)
    );
}


void BattleAction::Perform(Unit* actor, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const {
    std::vector<Unit*> targets = TargetManager::SelectTargets(actor, allies, enemies, *this);

    for (Unit* target : targets) {
        ActionContext ctx{ actor, target, allies, enemies };

        for (const auto& [condition, action] : conditionalActions) {
            if (condition(ctx)) {
                action(ctx);
            }
        }
    }
}
