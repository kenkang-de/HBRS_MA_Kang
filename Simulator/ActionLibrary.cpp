#include "ActionLibrary.h"
#include "Unit.h"
#include "BoonAction.h"
#include <algorithm>
#include <iostream>

// Global registry for loaded BattleActions (set by BattleActionLoader)
static std::unordered_map<std::string, BattleAction*> globalActionRegistry;

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
    //Check Target is killed 
    { "C05", [](const ActionContext& ctx) {
      return !ctx.target->IsAlive();
    }},
    //Check Target's speed is 0 
    { "C06", [](const ActionContext& ctx) {
      return ctx.target->GetTotalStat().GetSpeed() == 0;
    }},
    //Check Target's speed is not 0 
    { "C07", [](const ActionContext& ctx) {
      return ctx.target->GetTotalStat().GetSpeed() != 0;
    }},
    //Check Target does NOT have Poison debuff
    { "C08", [](const ActionContext& ctx) {
      return !HasBoonOnUnit(ctx.target, "Poison");
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
    //Add Target's speed to damage and attack.
    { "A21", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.target->GetTotalStat().GetSpeed(), true);
        }
    }},

    //Lower target's threat by target's defense. 
    { "A22", [](const ActionContext& ctx) {
        if (ctx.actor && ctx.target) {
            ctx.target->GetTotalStat().SetThreat(ctx.target->GetTotalStat().GetThreat() - ctx.target->GetTotalStat().GetDefense());
        }
    }},
    //Deals doubled damage
    { "A23", [](const ActionContext& ctx) {
     if (ctx.actor && ctx.target) {
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack()*2, true);
        }
    }},
    //Deals pierce damage (Actor's Attack - Target's speed)
    { "A24", [](const ActionContext& ctx) {
     if (ctx.actor && ctx.target) {
            ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetSpeed(), false);
        }
    }},
    //lower's enemies attack by damage * 0.5 (needs to be used after damage dealing action)
    { "A25", [](const ActionContext& ctx) {
     if (ctx.actor && ctx.target) {
            // Calculate the damage that would be dealt (before defense)
            int rawDamage = ctx.actor->GetTotalStat().GetAttack();
            int defense = ctx.target->GetTotalStat().GetDefense();
            int actualDamage = std::max(0, rawDamage - defense);  // Damage after defense
            
            int attackReduction = static_cast<int>(actualDamage * 0.5);  // Truncate (floor) to integer
            if (attackReduction > 0) {
                int oldAttack = ctx.target->GetTotalStat().GetAttack();
                ctx.target->GetTotalStat().SetAttack(oldAttack - attackReduction);
                int newAttack = ctx.target->GetTotalStat().GetAttack();
                std::cout << "[A25] " << ctx.target->GetName() << " attack reduced from " << oldAttack << " to " << newAttack << " (damage: " << actualDamage << ", reduction: " << attackReduction << ")" << std::endl;
            }
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

        //Store After Action - queue the specified action to be executed later
        { "A18", [](const std::string& param) -> ActionFn {
        std::string actionID = param;
        return [actionID](const ActionContext& ctx) {
            if (ctx.actor) {
                // Look up the after-action by ID
                const BattleAction* afterAction = ActionLibrary::GetGlobalAction(actionID);
                if (afterAction) {
                    // Register the after-action to be executed later
                    extern void AddAfterActionToBattleManager(const BattleAction* action, const ActionContext& context);
                    AddAfterActionToBattleManager(afterAction, ctx);
                    std::cout << "[A18] Queuing after-action: " << actionID << " for " << ctx.actor->GetName() << std::endl;
                } else {
                    std::cout << "[A18] ERROR: After-action not found: " << actionID << std::endl;
                }
            }
        };
    }},

        //Add to Actor's Speed
        { "A19", [](const std::string& param) -> ActionFn {
           int value = std::stoi(param);
        return [value](const ActionContext& ctx) {
            if (ctx.actor) {
                int oldSpeed = ctx.actor->GetTotalStat().GetSpeed();
                ctx.actor->GetTotalStat().SetSpeed(oldSpeed + value);
                int newSpeed = ctx.actor->GetTotalStat().GetSpeed();
                std::cout << "[A19] " << ctx.actor->GetName() << " speed changed from " << oldSpeed << " to " << newSpeed << std::endl;
            }
        };
    }},
    
        //Delay Target Unit
        { "A20", [](const std::string& param) -> ActionFn {
           int delayAmount = std::stoi(param);
        return [delayAmount](const ActionContext& ctx) {
            if (ctx.target) {
                extern void DelayUnitInBattleManager(Unit* unit, int delayAmount);
                DelayUnitInBattleManager(ctx.target, delayAmount);
                std::cout << "[A20] " << ctx.target->GetName() << " delayed by " << delayAmount << " ticks" << std::endl;
            }
        };
    }},
    
    //Special action to directly execute any registered effect action
    { "EXECUTE_EFFECT", [](const std::string& param) -> ActionFn {
        // param is the effect name to execute
        std::string effectName = param;
        
        return [effectName](const ActionContext& ctx) {
            // Get and execute the registered effect action directly
            const BattleAction* effectAction = ActionLibrary::GetGlobalAction(effectName);
            if (effectAction) {
                std::cout << "[EXECUTE_EFFECT] " << ctx.actor->GetName() << " triggered " << effectName << std::endl;
                effectAction->Perform(ctx.actor, ctx.target, ctx.allies, ctx.enemies);
            }
        };
    }},
    
    //Apply Poison Boon (param = usage)
    { "A26", [](const std::string& param) -> ActionFn {
        int usage = std::stoi(param);
        return [usage](const ActionContext& ctx) {
            if (ctx.target) {
                auto poisonBoon = std::make_unique<BoonAction>("Poison", "Poison", usage);
                poisonBoon->AddConditionalAction("C00", "A03", "3"); // Deal 3 undefendable damage per turn
                
                AddBoonToUnit(ctx.target, std::move(poisonBoon));
                std::cout << "[A26] " << ctx.actor->GetName() << " applied Poison to " << ctx.target->GetName() << " for " << usage << " uses" << std::endl;
            }
        };
    }},
    
    //Apply Boon with Effect (param format: "EffectName,usage" e.g., "Poison Effect,3")
    { "A28", [](const std::string& param) -> ActionFn {
        // Parse parameters: EffectName,usage (e.g., "Poison Effect,3")
        size_t comma = param.find(',');
        std::string effectName = param.substr(0, comma);
        int usage = std::stoi(param.substr(comma + 1));
        
        return [effectName, usage](const ActionContext& ctx) {
            if (ctx.target) {
                // Get the registered effect action from globalActionRegistry
                const BattleAction* effectAction = ActionLibrary::GetGlobalAction(effectName);
                if (effectAction) {
                    // Create a boon that will execute the specified effect action
                    auto boon = std::make_unique<BoonAction>(effectName, effectName, usage);
                    
                    // Add a conditional action that directly executes the registered effect
                    boon->AddConditionalAction("C00", "EXECUTE_EFFECT", effectName);
                    
                    AddBoonToUnit(ctx.target, std::move(boon));
                    std::cout << "[A28] " << ctx.actor->GetName() << " applied " << effectName << " to " << ctx.target->GetName() 
                              << " (usage: " << usage << ")" << std::endl;
                } else {
                    std::cout << "[A28] ERROR: Effect action '" << effectName << "' not found in registry!" << std::endl;
                }
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

// Global action registry functions
void ActionLibrary::RegisterGlobalAction(const std::string& id, BattleAction* action) {
    globalActionRegistry[id] = action;
}

const BattleAction* ActionLibrary::GetGlobalAction(const std::string& id) {
    auto it = globalActionRegistry.find(id);
    return (it != globalActionRegistry.end()) ? it->second : nullptr;
}

// Global functions for boon system
void AddBoonToUnit(Unit* target, std::unique_ptr<BoonAction> boon) {
    if (target) {
        target->AddBoon(std::move(boon));
    }
}

bool HasBoonOnUnit(Unit* target, const std::string& effectType) {
    return target ? target->HasBoon(effectType) : false;
}



