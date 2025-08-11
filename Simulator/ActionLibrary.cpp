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
    //Actor's Attack is higher than 0
    { "C04", [](const ActionContext& ctx) {
        return ctx.actor->GetTotalStat().GetAttack() > 0;  
    }},
    //When by attack kills a Unit
    { "C05", [](const ActionContext& ctx) {
        int actorAttack = ctx.actor->GetTotalStat().GetAttack();
        int targetDefense = ctx.target->GetTotalStat().GetDefense();
        int targetHP = ctx.target->GetCurrentHP();
        int damage = actorAttack - targetDefense;
        bool willKill = damage >= targetHP;
        
        std::cout << "[C05] " << ctx.actor->GetName() << " vs " << ctx.target->GetName() 
                  << " | ATK:" << actorAttack << " - DEF:" << targetDefense << " = " << damage 
                  << " | Target HP:" << targetHP << " | Will Kill: " << (willKill ? "YES" : "NO") << std::endl;
        
        return willKill;
    }},
    //When attack would kill target from full HP (true kill, not finishing blow)
    { "C06", [](const ActionContext& ctx) {
        int actorAttack = ctx.actor->GetTotalStat().GetAttack();
        int targetDefense = ctx.target->GetTotalStat().GetDefense();
        int targetMaxHP = ctx.target->GetTotalStat().GetHP();
        int damage = actorAttack - targetDefense;
        bool wouldKillFromFull = damage >= targetMaxHP;
        
        std::cout << "[C06] " << ctx.actor->GetName() << " vs " << ctx.target->GetName() 
                  << " | ATK:" << actorAttack << " - DEF:" << targetDefense << " = " << damage 
                  << " | Target Max HP:" << targetMaxHP << " | Would Kill From Full: " << (wouldKillFromFull ? "YES" : "NO") << std::endl;
        
        return wouldKillFromFull;
    }}
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
    //Adds actor's Defense to damage and attack.
    { "A11", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.actor->GetTotalStat().GetDefense(), true);
        }
    }},
    //Delete Actor's Defense
    { "A12", [](const ActionContext& ctx) {
        if (ctx.actor) {
            int oldDefense = ctx.actor->GetTotalStat().GetDefense();
            ctx.actor->GetTotalStat().SetDefense(0);
            int newDefense = ctx.actor->GetTotalStat().GetDefense();
            std::cout << "[A12] " << ctx.actor->GetName() << " defense changed from " << oldDefense << " to " << newDefense << std::endl;
        }
    }},
    //Leech damage 
    { "A14", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            int simulatedDamage = ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetDefense();
            if(simulatedDamage>0)
                ctx.actor->Heal(simulatedDamage);
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
       //Add to Actor's Attack
        { "A04", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor) {
                int oldAttack = ctx.actor->GetTotalStat().GetAttack();
                ctx.actor->GetTotalStat().SetAttack(oldAttack + value);
                int newAttack = ctx.actor->GetTotalStat().GetAttack();
                std::cout << "[A04] " << ctx.actor->GetName() << " attack changed from " << oldAttack << " to " << newAttack << std::endl;
            }
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
         //Add to actor's threat(param)
        { "A10", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor) {
                int oldThreat = ctx.actor->GetTotalStat().GetThreat();
                ctx.actor->GetTotalStat().SetThreat(oldThreat + value);
                int newThreat = ctx.actor->GetTotalStat().GetThreat();
                std::cout << "[A10] " << ctx.actor->GetName() << " threat changed from " << oldThreat << " to " << newThreat << std::endl;
            }
        };
    }},
        //Param - Damage is added to Actor's HP.
        { "A13", [](const std::string& param) -> ActionFn {
        int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            int simulatedDamage = ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetDefense();
            if (simulatedDamage > 0 && simulatedDamage < value) {
                ctx.actor->EnhanceHP(value-simulatedDamage);
            }
        };
    }},

        //Add to Actor's Defense
        { "A15", [](const std::string& param) -> ActionFn {
           int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor) {
                int oldDefense = ctx.actor->GetTotalStat().GetDefense();
                ctx.actor->GetTotalStat().SetDefense(oldDefense + value);
                int newDefense = ctx.actor->GetTotalStat().GetDefense();
                std::cout << "[A15] " << ctx.actor->GetName() << " defense changed from " << oldDefense << " to " << newDefense << std::endl;
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



