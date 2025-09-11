#ifndef ACTIONLIBRARY_H
#define ACTIONLIBRARY_H

class Unit;
class BattleAction;
class BoonAction;
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

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

    // Global action registry for A18 after-action system - using raw pointers for performance
    static void RegisterGlobalAction(const std::string& id, BattleAction* action);
    static const BattleAction* GetGlobalAction(const std::string& id);

private:
    static const std::unordered_map<std::string, ConditionFn> conditionMap;
    static const std::unordered_map<std::string, ActionFn> actionMap;
};


void DelayUnitInBattleManager(Unit* unit, int delayAmount);

// Global functions for Boon system
void AddBoonToUnit(Unit* target, std::unique_ptr<BoonAction> boon);
bool HasBoonOnUnit(Unit* target, const std::string& effectType);

#endif
