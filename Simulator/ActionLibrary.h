#ifndef ACTIONLIBRARY_H
#define ACTIONLIBRARY_H

class Unit;
class BattleAction;
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

struct ActionContext {
    Unit* actor;
    Unit* target;
    const std::vector<Unit*>& allies;
    const std::vector<Unit*>& enemies;
};

using ConditionFn = std::function<bool(const ActionContext&)>;
using ActionFn = std::function<void(const ActionContext&)>;

class ActionLibrary {
public:
    static ConditionFn GetCondition(const std::string& id);

    static ActionFn GetAction(const std::string& id);
    static ActionFn GetAction(const std::string& id, const std::string& param);

    // Global action registry for A18 after-action system
    static void RegisterGlobalAction(const std::string& id, BattleAction* action);
    static const BattleAction* GetGlobalAction(const std::string& id);

private:
    static const std::unordered_map<std::string, ConditionFn> conditionMap;
    static const std::unordered_map<std::string, ActionFn> actionMap;
};

#endif
