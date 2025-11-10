#include <iostream>
#include <unordered_map>

#include "ActionLibrary.h"
#include "GlobalAction.h"
#include "Unit.h"

static std::unordered_map<std::string, BattleAction *> globalActionRegistry;

std::vector<AfterActionEvent> GlobalAction::afterActions;

void GlobalAction::RegisterGlobalAction(const std::string &id, BattleAction *action) {
    globalActionRegistry[id] = action;
}

BattleAction *GlobalAction::GetGlobalAction(const std::string &id) {
    auto it = globalActionRegistry.find(id);
    return (it != globalActionRegistry.end()) ? it->second : nullptr;
}

void GlobalAction::AddAfterAction(BattleAction *battleAction, ActionContext context) {
    afterActions.emplace_back(battleAction, context);
}

void GlobalAction::ClearAfterAction() {
    afterActions.clear();
}

void GlobalAction::ClearGlobalRegistry() {
    globalActionRegistry.clear();
}