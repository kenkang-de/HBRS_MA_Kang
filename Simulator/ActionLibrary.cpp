#include "ActionLibrary.h"
#include "Unit.h"
#include <algorithm>
#include <iostream>

const std::unordered_map<std::string, ConditionFn> ActionLibrary::conditionMap = {

    { "TargetLowHP", [](const ActionContext& ctx) {
        return ctx.target && ctx.target->GetCurrentHP() < 30;
    }},
    { "ActorHasMoreAttackThanTargetDefense", [](const ActionContext& ctx) {
        return ctx.actor && ctx.target &&
               ctx.actor->GetTotalStat().GetAttack() > ctx.target->GetTotalStat().GetDefense();
    }},
    { "AnyAllyLowHP", [](const ActionContext& ctx) {
        return std::any_of(ctx.allies.begin(), ctx.allies.end(), [&](Unit* u) {
            return u != ctx.actor && u->GetCurrentHP() < 30;
        });
    }},
    //ALWAYS
    { "C00", [](const ActionContext&) {
        return true;
    }},
    //Actor Attack is Higher than Target's Defense.
    { "C01", [](const ActionContext& ctx) {
        return ctx.actor->GetTotalStat().GetAttack() > ctx.target->GetTotalStat().GetDefense();  
    }},
    //Actor Attack is same or lower than Target's Defense
    { "C02", [](const ActionContext& ctx) {
    return ctx.actor->GetTotalStat().GetAttack() <= ctx.target->GetTotalStat().GetDefense();  
    }},
    //Actor's HP is higher than Target's HP
    { "C03", [](const ActionContext& ctx) {
        return ctx.actor->GetCurrentHP() > ctx.target->GetCurrentHP();
    }},
};

const std::unordered_map<std::string, ActionFn> ActionLibrary::actionMap = {
    //Deal Damage
    { "A00", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            // Magic damage ignores defense
            bool defendable = (ctx.actor->GetWeapon().GetAction().GetActionType() != ActionType::MAGIC);
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack(), defendable);
        }
    }},
    //Deal Crit Damage
    { "A01", [](const ActionContext& ctx) {
     if (ctx.actor && ctx.target) {
            // Magic damage ignores defense
            bool defendable = (ctx.actor->GetWeapon().GetAction().GetActionType() != ActionType::MAGIC);
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack()*2, defendable);
        }
    }},
    //At Attack, 1/3 of current HP + Speed of the actor is added to damage
    { "A07", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            // Use floating-point division then cast to int (truncates towards zero)
            int additionalDamage = static_cast<int>(ctx.actor->GetCurrentHP() / 3.0) + ctx.actor->GetTotalStat().GetSpeed();
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + additionalDamage, true);
        }
    }},
    //Adds actor's Speed to damage and attack.
    { "A08", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.actor->GetTotalStat().GetSpeed(), true);
        }
    }},
};

using ParamActionFactory = std::function<ActionFn(const std::string& param)>;

static const std::unordered_map<std::string, ParamActionFactory> paramActionFactory = {

        //Lowers Target's Defense
    { "A02", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.target)
                ctx.target->GetTotalStat().SetDefense(ctx.target->GetTotalStat().GetDefense() + value);
        };
    }},
        //Undefendable Static damage
        { "A03", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.target)
                ctx.target->TakeDamage(value, false);
        };
    }},
       //Increase Actor's Attack
        { "A04", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor)
                ctx.actor->GetTotalStat().SetAttack(ctx.actor->GetTotalStat().GetAttack() + value);
        };
    }},
        //Actor Takes damage(undefendable)
        { "A05", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor)
                ctx.actor->TakeDamage(value, false);
        };
    }},
        //Change Speed of a target
        { "A06", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.target) {
                int oldSpeed = ctx.target->GetTotalStat().GetSpeed();
                ctx.target->GetTotalStat().SetSpeed(oldSpeed + value);
                int newSpeed = ctx.target->GetTotalStat().GetSpeed();
                std::cout << "[A06] " << ctx.target->GetName() << " speed changed from " << oldSpeed << " to " << newSpeed << std::endl;
            }
        };
    }},
         //Attack with increased damage(param)
        { "A09", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.target) {
                ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + value, true);
            }
        };
    }},
         //Lower actor's threat(param)
        { "A10", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor) {
                int oldThreat = ctx.actor->GetTotalStat().GetThreat();
                ctx.actor->GetTotalStat().SetThreat(oldThreat - value);
                int newThreat = ctx.actor->GetTotalStat().GetThreat();
                std::cout << "[A10] " << ctx.actor->GetName() << " threat changed from " << oldThreat << " to " << newThreat << std::endl;
            }
        };
    }},


};


ConditionFn ActionLibrary::GetCondition(const std::string& id) {
    auto it = conditionMap.find(id);
    if (it != conditionMap.end()) return it->second;

    return [](const ActionContext&) { return false; };  // Fallback
}

ActionFn ActionLibrary::GetAction(const std::string& id) {
    auto it = actionMap.find(id);
    if (it != actionMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown action ID: " + id);
}

ActionFn ActionLibrary::GetAction(const std::string& id, const std::string& param) {
    if (param.empty()) {
        return GetAction(id);  // Use static action map
    }

    auto it = paramActionFactory.find(id);
    if (it != paramActionFactory.end()) {
        return it->second(param);
    }

    throw std::runtime_error("No parameterized action for ID: " + id);
}



