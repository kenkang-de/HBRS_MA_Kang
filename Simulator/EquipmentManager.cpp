#include "EquipmentManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "BattleAction.h"

#include "Unit.h"
#include <random>
#include <chrono>

std::vector<Armor> ArmorList;
std::vector<Weapon> WeaponList;

#include <algorithm>
#include <cctype>

static std::string Trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

void LoadArmorListFromCSV(const std::string& filepath)
{
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

        std::getline(ss, token, ','); atk = std::stoi(token);
        std::getline(ss, token, ','); def = std::stoi(token);
        std::getline(ss, token, ','); spd = std::stoi(token);
        std::getline(ss, token, ','); thr = std::stoi(token);
        std::getline(ss, token, ','); hp  = std::stoi(token);

        Stat stat(atk, def, hp, spd, thr);
        Armor armor(id, name, stat);
        ArmorList.push_back(armor);
    }
}


void LoadWeaponListFromCSV(const std::string& filepath, const std::unordered_map<std::string, BattleAction>& actionMap)
{
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

        std::getline(ss, token, ','); atk = std::stoi(token);
        std::getline(ss, token, ','); def = std::stoi(token);
        std::getline(ss, token, ','); spd = std::stoi(token);
        std::getline(ss, token, ','); thr = std::stoi(token);
        std::getline(ss, token, ','); hp  = std::stoi(token);
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
        WeaponList.push_back(weapon);
    }
}



// Modern random number generator
static std::random_device rd;
static std::mt19937 gen(rd());

Armor GetRandomArmor()
{
    if (ArmorList.empty()) {
        throw std::runtime_error("ArmorList is empty");
    }

    std::uniform_int_distribution<> dis(0, ArmorList.size() - 1);
    int index = dis(gen);
    return ArmorList[index];
}

Weapon GetRandomWeapon()
{
    if (WeaponList.empty()) {
        throw std::runtime_error("WeaponList is empty");
    }

    std::uniform_int_distribution<> dis(0, WeaponList.size() - 1);
    int index = dis(gen);
    return WeaponList[index];
}

void EquipUnitsRandomEquipments(std::list<Unit>& unitList)
{
    for(Unit& unit: unitList)
    {
        Weapon randomWeapon = GetRandomWeapon();
        Armor randomArmor = GetRandomArmor();
        
        unit.SetWeapon(randomWeapon);
        unit.SetArmor(randomArmor);
    }
}

void ReseedRandomGenerator()
{
    // Use high-resolution clock for better entropy
    auto now = std::chrono::high_resolution_clock::now();
    auto seed = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    gen.seed(static_cast<std::mt19937::result_type>(seed));
}
