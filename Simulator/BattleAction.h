#ifndef ACTION_H
#define ACTION_H

#include "ActionLibrary.h" 

#include <string>
#include <vector>

class Unit;

enum TargetType {ENEMY, ALLY };
enum ActionType { MELEE, RANGE, MAGIC };
class BattleAction{

private: 
int targetNumber;
bool self;
TargetType targetType;
ActionType actionType;
std::string ID;

std::vector<std::pair<ConditionFn, ActionFn>> conditionalActions;

public: 
 BattleAction() : targetNumber(0), self(false), targetType(ENEMY), ID("None") {}

 BattleAction(int _targetNumber, bool _self, TargetType _targetType, std::string id)
        : targetNumber(_targetNumber), self(_self),  targetType(_targetType), ID(id) {}

const std::string& GetID() const;

void Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const;

int CalculateDamage(Unit* actingUnit, Unit* target) const;


void SetActionType(ActionType type) { actionType = type; }
ActionType GetActionType() const { return actionType; }

TargetType GetTargetType() const { return targetType; }
bool IncludesSelf() const { return self; }
int GetTargetNumber() const { return targetNumber; }

void AddConditionalAction(const std::string& conditionID, const std::string& actionID, const std::string& param);

};

#endif