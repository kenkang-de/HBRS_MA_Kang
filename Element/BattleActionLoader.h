#ifndef BATTLEACTIONLOADER_H
#define BATTLEACTIONLOADER_H

#include "../Simulator/BattleAction.h"
#include <unordered_map>
#include <string>

std::unordered_map<std::string, BattleAction> LoadActionsFromYAML(const std::string& filepath);

#endif
