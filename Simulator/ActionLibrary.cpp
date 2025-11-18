#include <algorithm>
#include <iostream>

#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "BoonAction.h"
#include "GlobalAction.h"
#include "TempBoonAction.h"
#include "Unit.h"

const std::unordered_map<std::string, ConditionFn> ActionLibrary::conditionMap = {
    // ALWAYS
    {"C00", [](const ActionContext &) { return true; }},
    // Actor Attack is Higher than Target's Defense.
    {"C01",
     [](const ActionContext &ctx) {
         return ctx.actor->GetTotalStat().GetAttack() > ctx.target->GetTotalStat().GetDefense();
     }},
    // Actor Attack is same or lower than Target's Defense
    {"C02",
     [](const ActionContext &ctx) {
         return ctx.actor->GetTotalStat().GetAttack() <= ctx.target->GetTotalStat().GetDefense();
     }},
    // Actor's Total HP is higher than Target's Total HP
    {"C03",
     [](const ActionContext &ctx) { return ctx.actor->GetTotalStat().GetHP() > ctx.target->GetTotalStat().GetHP(); }},
    // Actor's Attack is higher than 0
    {"C04", [](const ActionContext &ctx) { return ctx.actor->GetTotalStat().GetAttack() > 0; }},
    // Check Target is killed
    {"C05", [](const ActionContext &ctx) { return !ctx.target->IsAlive(); }},
    // Check Target's speed is 0
    {"C06", [](const ActionContext &ctx) { return ctx.target->GetTotalStat().GetSpeed() == 0; }},
    // Check Target's speed is not 0
    {"C07", [](const ActionContext &ctx) { return ctx.target->GetTotalStat().GetSpeed() != 0; }},
    // Check Target does NOT have Poison debuff
    {"C08", [](const ActionContext &ctx) { return !HasBoonOnUnit(ctx.target, "Poison"); }}};

const std::unordered_map<std::string, ActionFn> ActionLibrary::actionMap = {
    // Deal Damage
    {"A00",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target && ctx.actor->GetWeapon()) {
             // Magic damage ignores defense
             bool defendable = (ctx.actor->GetWeapon()->GetAction().GetActionType() != ActionType::MAGIC);
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack(), defendable, ctx.actor);
         }
     }},
    // Deal Crit Damage
    {"A01",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target && ctx.actor->GetWeapon()) {
             // Magic damage ignores defense
             bool defendable = (ctx.actor->GetWeapon()->GetAction().GetActionType() != ActionType::MAGIC);
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() * 2, defendable, ctx.actor);
         }
     }},
    // At Attack, 1/3 of current HP  of the actor is added to damage
    {"A07",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             // Use floating-point division then cast to int (truncates towards zero)
             int additionalDamage = static_cast<int>(ctx.actor->GetCurrentHP() * 0.33f);
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + additionalDamage, true, ctx.actor);
         }
     }},
    // Adds actor's Speed to damage and attack.
    {"A08",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.actor->GetTotalStat().GetSpeed(), true,
                                    ctx.actor);
         }
     }},
    // Adds actor's Defense to damage and attack.
    {"A11",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.actor->GetTotalStat().GetDefense(),
                                    true, ctx.actor);
         }
     }},
    // Delete Actor's Defense
    {"A12",
     [](const ActionContext &ctx) {
         if (ctx.actor) {
             int oldDefense = ctx.actor->GetTotalStat().GetDefense();
             ctx.actor->GetTotalStat().SetDefense(0);
             int newDefense = ctx.actor->GetTotalStat().GetDefense();
             LogSystem::LogStream("[A12] ", ctx.actor->GetName(), " defense changed from ", oldDefense, " to ",
                                  newDefense);
         }
     }},
    // Leech damage
    {"A14",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int simulatedDamage = ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetDefense();
             if (simulatedDamage > 0)
                 ctx.actor->Heal(simulatedDamage);
         }
     }},
    // Add Target's speed to damage and attack.
    {"A21",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() + ctx.target->GetTotalStat().GetSpeed(), true,
                                    ctx.actor);
         }
     }},

    // Lower target's threat by target's defense.
    {"A22",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->GetTotalStat().SetThreat(ctx.target->GetTotalStat().GetThreat() -
                                                  ctx.target->GetTotalStat().GetDefense());
         }
     }},

    // Deals doubled damage
    {"A23",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() * 2, true, ctx.actor);
         }
     }},

    // Deals pierce damage (Actor's Attack - Target's speed)
    {"A24",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetSpeed(),
                                    false, ctx.actor);
         }
     }},

    // lower's enemies attack by damage * 0.5 (needs to be used after damage dealing action)
    {"A25",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             // Calculate the damage that would be dealt (before defense)
             int rawDamage = ctx.actor->GetTotalStat().GetAttack();
             int defense = ctx.target->GetTotalStat().GetDefense();
             int actualDamage = std::max(0, rawDamage - defense); // Damage after defense

             int attackReduction = static_cast<int>(actualDamage * 0.5); // Truncate (floor) to integer
             if (attackReduction > 0) {
                 int oldAttack = ctx.target->GetTotalStat().GetAttack();
                 ctx.target->GetTotalStat().SetAttack(oldAttack - attackReduction);
                 int newAttack = ctx.target->GetTotalStat().GetAttack();
                 LogSystem::LogStream("[A25] ", ctx.target->GetName(), " attack reduced from ", oldAttack, " to ",
                                      newAttack, " (damage: ", actualDamage, ", reduction: ", attackReduction, ")");
             }
         }
     }},

    // Freeze target
    {"A30",
     [](const ActionContext &ctx) {
         if (ctx.target) {
             ctx.target->SetFrozen(true);
         }
     }},

    // Unfreeze target
    {"A35",
     [](const ActionContext &ctx) {
         if (ctx.target) {
             ctx.target->SetFrozen(false);
         }
     }},

    // Raise Target's Defense by (actor's Threat)
    {"A02",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->GetTotalStat().SetDefense(ctx.target->GetTotalStat().GetDefense() +
                                                   ctx.actor->GetTotalStat().GetThreat());
         }
     }},

    // Deals undefendable damage to target by (actor's Attack)
    {"A03",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->TakeDamage(ctx.actor->GetTotalStat().GetAttack(), false, ctx.actor);
         }
     }},

    // Add actor's attack by (actor's HP-Defense)
    {"A04",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = ctx.actor->GetTotalStat().GetHP() - ctx.actor->GetTotalStat().GetDefense();
             value = std::max(0,value);
             ctx.actor->GetTotalStat().SetAttack(ctx.actor->GetTotalStat().GetAttack() + value);
         }
     }},

    // Deals undefendable damage to self (actor's Defense)
    {"A05",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
            int value = std::max(0,ctx.actor->GetTotalStat().GetDefense());
             ctx.actor->TakeDamage(value, false, ctx.actor);
         }
     }},

    // Change Speed of a target by (actor's Speed-Threat)
    {"A06",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = ctx.actor->GetTotalStat().GetSpeed() - ctx.actor->GetTotalStat().GetThreat();
             int newSpeed = ctx.target->GetTotalStat().GetSpeed() + value;
             // Ensure speed never goes below 1 to prevent infinite loops
             ctx.target->SetSpeed(std::max(1, newSpeed));
         }
     }},

    // Attack with increased damage (1.25 * actor's Attack)
    {"A09",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
            int damage = static_cast<int>(ctx.actor->GetTotalStat().GetAttack() * 1.25f);
            ctx.target->TakeDamage(damage, false, ctx.actor);
         }
     }},

    // Change Threat of actor (actor's Defense - Attack)
    {"A10",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = ctx.actor->GetTotalStat().GetDefense() - ctx.actor->GetTotalStat().GetAttack();
             ctx.actor->GetTotalStat().SetThreat(ctx.actor->GetTotalStat().GetThreat() + value);
         }
     }},

    // Damage added to actor's HP
    {"A13",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = ctx.actor->GetTotalStat().GetAttack() - ctx.target->GetTotalStat().GetDefense();
             ctx.actor->GetTotalStat().SetHP(ctx.actor->GetTotalStat().GetHP() + value);
         }
     }},

    // Add to actor's Defense (actor's Speed / 2)
    {"A15",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = static_cast<int>(ctx.actor->GetTotalStat().GetSpeed() * 0.5f);
             ctx.actor->GetTotalStat().SetDefense(ctx.actor->GetTotalStat().GetDefense() + value);
         }
     }},

    // Add to actor's Speed ( actor's HP / 10)
    {"A19",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = static_cast<int>(ctx.actor->GetTotalStat().GetHP() * 0.1f);
             ctx.actor->SetSpeed(ctx.actor->GetTotalStat().GetSpeed() + value);
         }
     }},

    // Delay Target Unit (actor's Threat * 0.25 )
    {"A20",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int totalDelay = static_cast<int>(ctx.actor->GetTotalStat().GetThreat() * 0.25f) * ctx.target->Tickinterval;
             ctx.target->TickDelay += totalDelay;
         }
     }},

    // Apply poison boon (usage = actor's Speed- target's Speed)
    {"A26",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int usage = std::max(1, ctx.actor->GetTotalStat().GetSpeed() - ctx.target->GetTotalStat().GetSpeed());
             auto poisonBoon = std::make_unique<BoonAction>("Poison", "Poison", usage);
             poisonBoon->AddConditionalAction("C00", "A03", std::to_string(ctx.actor->GetTotalStat().GetAttack()));
             AddBoonToUnit(ctx.target, std::move(poisonBoon));
         }
     }},

    // Heal target (actor's Attack)
    {"A29",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->Heal(ctx.actor->GetTotalStat().GetAttack());
         }
     }},
    // Ehance target's Health (actor's Attack)
    {"A32",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
                ctx.target->EnhanceHP(ctx.actor->GetTotalStat().GetAttack());
         }
     }},

    // Decrease target's Health (actor's Attack)
    {"A36",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
                ctx.target->EnhanceHP(-ctx.actor->GetTotalStat().GetAttack());
         }
     }},

    // Add target's threat (actor's Attack)
    {"A33",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
            int value = ctx.actor->GetTotalStat().GetAttack();
            ctx.target->GetTotalStat().SetThreat(ctx.target->GetTotalStat().GetThreat() + value);
         }
     }},

    // subtract target's attack (actor's Attack)
    {"A34",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
            int value = ctx.actor->GetTotalStat().GetAttack();
    ctx.target->GetTotalStat().SetAttack(ctx.target->GetTotalStat().GetAttack() - value);
         }
     }},

    // Decrease target's speed (actor's HP*0.1)
    {"A37",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             int value = static_cast<int>(ctx.actor->GetTotalStat().GetHP() * 0.1f * -1);
             int newSpeed = ctx.actor->GetTotalStat().GetSpeed() + value;
             // Ensure speed never goes below 1 to prevent infinite loops
             ctx.target->SetSpeed(std::max(1, newSpeed));
         }
     }},

    // Decrease Target's Defense by (actor's Threat)
    {"A38",
     [](const ActionContext &ctx) {
         if (ctx.actor && ctx.target) {
             ctx.target->GetTotalStat().SetDefense(ctx.target->GetTotalStat().GetDefense() -
                                                   ctx.actor->GetTotalStat().GetThreat());
         }
     }},
     
     // Raise Actor's Attack by (actor's Defense) 
     {"A39",
     [](const ActionContext &ctx) {
         if (ctx.actor) {
            int value = std::max(0,ctx.actor->GetTotalStat().GetDefense());
             ctx.actor->GetTotalStat().SetAttack(ctx.actor->GetTotalStat().GetAttack() + value);
         }
     }},

    
    
};

using ParamActionFactory = std::function<ActionFn(const std::string &param)>;

static const std::unordered_map<std::string, ParamActionFactory> paramActionFactory = {

    // Store After Action - queue the specified action to be executed later
    {"A18",
     [](const std::string &param) -> ActionFn {
         std::string actionID = param;
         return [actionID](ActionContext ctx) {
             if (ctx.actor) {
                 auto afterAction = GlobalAction::GetGlobalAction(actionID);
                 if (afterAction) {
                     GlobalAction::AddAfterAction(afterAction, ctx);
                 } else {
                     std::cout << "[ERROR] Failed to get global action: " + actionID << std::endl;
                 }
             }
         };
     }},

    // Special action to directly execute any registered effect action
    {"EXECUTE_EFFECT",
     [](const std::string &param) -> ActionFn {
         std::string effectName = param;

         return [effectName](const ActionContext &ctx) {
             // Get and execute the registered effect action directly
             auto effectAction = GlobalAction::GetGlobalAction(effectName);
             if (effectAction) {
                 LogSystem::LogStream("[EXECUTE_EFFECT] ", ctx.actor->GetName(), " triggered ", effectName);
                 effectAction->Perform(ctx.actor, ctx.target);
             }
         };
     }},

    // Apply Boon with Effect (param format: "EffectName,usage" e.g., "Poison Effect,3")
    {"A28",
     [](const std::string &param) -> ActionFn {
         // Parse parameters: EffectName,usage (e.g., "Poison Effect,3")
         size_t comma = param.find(',');
         std::string effectName = param.substr(0, comma);
         int usage = std::stoi(param.substr(comma + 1));

         return [effectName, usage](const ActionContext &ctx) {
             if (ctx.target) {
                 // Get the registered effect action from globalActionRegistry
                 auto effectAction = GlobalAction::GetGlobalAction(effectName);
                 if (effectAction) {
                     // Create a boon that will execute the specified effect action
                     auto boon = std::make_unique<BoonAction>(effectName, effectName, usage);

                     // Add a conditional action that directly executes the registered effect
                     boon->AddConditionalAction("C00", "EXECUTE_EFFECT", effectName);

                     AddBoonToUnit(ctx.target, std::move(boon));
                     LogSystem::LogStream("[A28] ", ctx.actor->GetName(), " applied ", effectName, " to ",
                                          ctx.target->GetName(), " (usage: ", usage, ")");
                 } else {
                     LogSystem::LogStream("[A28] ERROR: Effect action '", effectName, "' not found in registry!");
                 }
             }
         };
     }},

    // Apply TempBoon with BattleAction names (param format: "StartActionName,duration,EndActionName")
    {"A31",
     [](const std::string &param) -> ActionFn {
         // Parse: "Bulkup(START),3,Bulkup(END)"
         size_t comma1 = param.find(',');
         size_t comma2 = param.find(',', comma1 + 1);

         if (comma1 == std::string::npos || comma2 == std::string::npos) {
             LogSystem::LogStream("[A31 ERROR] Invalid parameter format: ", param);
             return [](const ActionContext &) {};
         }

         std::string startActionName = param.substr(0, comma1); // "Bulkup(START)"
         std::string durationStr = param.substr(comma1 + 1, comma2 - comma1 - 1);
         std::string endActionName = param.substr(comma2 + 1); // "Bulkup(END)"

         int duration;
         try {
             duration = std::stoi(durationStr);
         } catch (const std::exception &e) {
             LogSystem::LogStream("[A31 ERROR] Failed to parse duration '", durationStr, "': ", e.what());
             return [](const ActionContext &) {};
         }

         return [startActionName, duration, endActionName](const ActionContext &ctx) {
             if (ctx.target) {
                 // Create TempBoonAction
                 auto tempBoon =
                     std::make_unique<TempBoonAction>(startActionName, startActionName, duration, endActionName);

                 // Add the buff effect using EXECUTE_EFFECT (no parameters needed)
                 tempBoon->AddConditionalAction("C00", "EXECUTE_EFFECT", startActionName);

                 AddBoonToUnit(ctx.target, std::move(tempBoon));
                 LogSystem::LogStream("[A31] Applied temporary buff ", startActionName, " for ", duration,
                                      " turns, removal: ", endActionName);
             }
         };
     }},

};

ConditionFn ActionLibrary::GetCondition(const std::string &id) {
    auto it = conditionMap.find(id);
    if (it != conditionMap.end())
        return it->second;

    return [](const ActionContext &) { return false; }; // Fallback
}

ActionFn ActionLibrary::GetAction(const std::string &id) {
    auto it = actionMap.find(id);
    if (it != actionMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown action ID: " + id);
}

ActionFn ActionLibrary::GetAction(const std::string &id, const std::string &param) {
    if (param.empty()) {
        return GetAction(id); // Use static action map
    }

    auto it = paramActionFactory.find(id);
    if (it != paramActionFactory.end()) {
        return it->second(param);
    }

    throw std::runtime_error("No parameterized action for ID: " + id);
}

// Global functions for boon system
void AddBoonToUnit(Unit *target, std::unique_ptr<BoonAction> boon) {
    if (target) {
        target->AddBoon(std::move(boon));
    }
}

bool HasBoonOnUnit(Unit *target, const std::string &effectType) {
    return target ? target->HasBoon(effectType) : false;
}
