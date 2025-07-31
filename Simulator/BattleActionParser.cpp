#include "BattleActionParser.h"

TargetType ParseTargetType(const std::string& str) {
    if (str == "ENEMY") return TargetType::ENEMY;
    if (str == "ALLY") return TargetType::ALLY;
    throw std::invalid_argument("Invalid TargetType string: " + str);
}

ActionType ParseActionType(const std::string& str) {
    if (str == "MELEE") return ActionType::MELEE;
    if (str == "RANGE") return ActionType::RANGE;
    if (str == "MAGIC") return ActionType::MAGIC;
    throw std::invalid_argument("Invalid ActionType string: " + str);
}
