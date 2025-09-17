#include <unordered_map>
#include <iostream>

#include "GlobalAction.h"
#include "ActionLibrary.h"
#include "Unit.h"

static std::unordered_map<std::string, BattleAction*> globalActionRegistry;

std::vector<AfterActionEvent> GlobalAction::afterActions;

void GlobalAction::RegisterGlobalAction(const std::string& id, BattleAction* action) {
    globalActionRegistry[id] = action;
}

const BattleAction* GlobalAction::GetGlobalAction(const std::string& id) {
    auto it = globalActionRegistry.find(id);
    return (it != globalActionRegistry.end()) ? it->second : nullptr;
}

void GlobalAction::AddAfterAction(const BattleAction* battleAction, const ActionContext& context) {
    afterActions.emplace_back(battleAction, context);
}

void GlobalAction::ClearAfterAction()
{
    afterActions.clear();
}