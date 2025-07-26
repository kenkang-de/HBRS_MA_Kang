#include "BattleActionParser.h"

TargetType ParseTargetType(const std::string& str) {
    if (str == "ENEMY") return TargetType::ENEMY;
    if (str == "ALLY") return TargetType::ALLY;
    throw std::invalid_argument("Invalid TargetType string: " + str);
}
