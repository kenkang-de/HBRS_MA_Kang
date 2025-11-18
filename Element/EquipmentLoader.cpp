#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Simulator/Armor.h"
#include "../Simulator/BattleAction.h"
#include "../Simulator/Weapon.h"
#include "ElementList.h"
#include "EquipmentLoader.h"
#include "Stat.h"

#include "Paths.h"

static std::string Trim(const std::string &str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start))
        start++;

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

void EquipmentLoader::LoadArmorListFromCSV(const std::string &filepath) {
    std::ifstream file(filepath);
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token, id, type, name;
        int atk, def, spd, thr, hp;

        std::getline(ss, id, ',');
        std::getline(ss, type, ',');
        std::getline(ss, name, ',');

        std::getline(ss, token, ',');
        atk = std::stoi(token);
        std::getline(ss, token, ',');
        def = std::stoi(token);
        std::getline(ss, token, ',');
        spd = std::stoi(token);
        std::getline(ss, token, ',');
        thr = std::stoi(token);
        std::getline(ss, token, ',');
        hp = std::stoi(token);

        Stat stat(atk, def, hp, spd, thr);
        Armor armor(id, name, stat);

        equipment.armors.emplace_back(armor);
    }
}

void EquipmentLoader::LoadWeaponListFromCSV(const std::string &filepath,
                                            const std::unordered_map<std::string, BattleAction> &actionMap) {
    std::ifstream file(filepath);
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token, id, type, name, actionId;
        int atk, def, spd, thr, hp;

        std::getline(ss, id, ',');
        std::getline(ss, type, ',');
        std::getline(ss, name, ',');

        std::getline(ss, token, ',');
        atk = std::stoi(token);
        std::getline(ss, token, ',');
        def = std::stoi(token);
        std::getline(ss, token, ',');
        spd = std::stoi(token);
        std::getline(ss, token, ',');
        thr = std::stoi(token);
        std::getline(ss, token, ',');
        hp = std::stoi(token);
        std::getline(ss, actionId, ',');
        actionId = Trim(actionId);

        Stat stat(atk, def, hp, spd, thr);

        // Look up the action
        BattleAction action;
        auto it = actionMap.find(actionId);
        if (it != actionMap.end()) {
            action = it->second;
        } else {
            std::cerr << "[WARNING] Unknown actionId: " << actionId << " for weapon: " << name << std::endl;
        }

        Weapon weapon(id, name, stat, action);
        equipment.weapons.emplace_back(weapon);
    }
}

ElementList EquipmentLoader::InstantiateElements(const std::unordered_map<std::string, BattleAction> &actionMap) {
    LoadArmorListFromCSV(Paths::DATA_DIR + Paths::ARMOR_DATA_CSV);
    LoadWeaponListFromCSV(Paths::DATA_DIR + Paths::WEAPON_DATA_CSV, actionMap);

    return equipment;
}

ElementList EquipmentLoader::InstantiateElements(const std::unordered_map<std::string, BattleAction> &actionMap,
                                                 std::string weaponFile, std::string armorFile) {
    LoadArmorListFromCSV(Paths::DATA_DIR + armorFile);
    LoadWeaponListFromCSV(Paths::DATA_DIR + weaponFile, actionMap);

    return equipment;
}