#include "BattleActionLoader.h"
#include "BattleAction.h"
#include "BattleActionParser.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <unordered_map>

std::unordered_map<std::string, BattleAction> LoadActionsFromYAML(const std::string& filename) {
    std::unordered_map<std::string, BattleAction> actionMap;

    YAML::Node root = YAML::LoadFile(filename);
    for (const auto& node : root) {
        std::string id = node["id"].as<std::string>();
        int targetNumber = node["targetNumber"].as<int>();
        bool self = node["self"].as<bool>();
        TargetType targetType = ParseTargetType(node["targetType"].as<std::string>());
        ActionType actionType = ParseActionType(node["actionType"].as<std::string>());

        BattleAction action(targetNumber, self, targetType, id);
        action.SetActionType(actionType);

        if (node["actionIDs"]) {
            for (const auto& actionIDStr : node["actionIDs"]) {
                std::string encoded = actionIDStr.as<std::string>();

            std::string condID, actID, param;

size_t c_pos = encoded.find('_');
size_t a_pos = encoded.find('_', c_pos + 1);
size_t p_start = encoded.find('(', a_pos + 1);
size_t p_end = encoded.find(')', p_start + 1);

if (a_pos == std::string::npos) {
    condID = encoded.substr(0, c_pos);
    actID = encoded.substr(c_pos + 1);
} else {
    condID = encoded.substr(c_pos + 1, a_pos - c_pos - 1);
    actID = encoded.substr(a_pos + 1);
}

if (p_start != std::string::npos && p_end != std::string::npos) {
    param = encoded.substr(p_start + 1, p_end - p_start - 1);
    actID = actID.substr(0, actID.find('(')); // strip param from action ID
}

action.AddConditionalAction(condID, actID, param);

            }
        }

        actionMap[id] = action;
    }

    return actionMap;
}
