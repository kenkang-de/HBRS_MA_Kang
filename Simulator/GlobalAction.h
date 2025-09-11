#ifndef GLOBAL_ACTION_H
#define GLOBAL_ACTION_H

#include <string>
#include <vector>

#include "BattleAction.h"
#include "GlobalAction.h"


struct AfterActionEvent {
    const BattleAction* battleAction;
    ActionContext context;
    
    AfterActionEvent(const BattleAction* action, const ActionContext& ctx)
        : battleAction(action), context(ctx) {}
};

class GlobalAction{

    public:
        static void RegisterGlobalAction(const std::string& id, BattleAction* action);
        static const BattleAction* GetGlobalAction(const std::string& id);

        static std::vector<AfterActionEvent> afterActions;
        static void AddAfterAction(const BattleAction* battleAction, const ActionContext& context);
};

#endif