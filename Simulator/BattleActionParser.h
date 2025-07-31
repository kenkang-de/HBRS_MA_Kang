// BattleActionParser.h
#pragma once
#include <string>
#include <stdexcept>
#include "BattleAction.h"

TargetType ParseTargetType(const std::string& str);
ActionType ParseActionType(const std::string& str);
