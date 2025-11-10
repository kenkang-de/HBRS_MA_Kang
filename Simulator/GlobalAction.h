#ifndef GLOBAL_ACTION_H
#define GLOBAL_ACTION_H

#include <string>
#include <vector>

#include "BattleAction.h"

struct AfterActionEvent {
    BattleAction *battleAction;
    ActionContext context;

    AfterActionEvent(BattleAction *action, ActionContext &ctx) : battleAction(action), context(ctx) {}
};

class GlobalAction {

  public:
    static void RegisterGlobalAction(const std::string &id, BattleAction *action);
    static BattleAction *GetGlobalAction(const std::string &id);

    static std::vector<AfterActionEvent> afterActions;
    static void AddAfterAction(BattleAction *battleAction, ActionContext context);
    static void ClearAfterAction();
    static void ClearGlobalRegistry(); // Add method to clear static registry
};

#endif