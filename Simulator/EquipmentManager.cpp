#include "EquipmentManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "BattleAction.h"

#include "Unit.h"
#include <cstdlib>   // for rand()
#include <ctime>     // for time()

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
        Armor armor(name, stat);
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
            std::cout << "[DEBUG] Loaded action: " << action.GetID() << " for weapon: " << name << std::endl;
        } else {
            std::cerr << "[WARNING] Unknown actionId: " << actionId << " for weapon: " << name << std::endl;
        }

Weapon weapon(name, stat, action);
        WeaponList.push_back(weapon);
    }
}



Armor GetRandomArmor()
{
    if (ArmorList.empty()) {
        throw std::runtime_error("ArmorList is empty");
    }

    // Seed rand() once globally — ideally done in main(), not here
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    // Generate random index
    int index = std::rand() % ArmorList.size();

    // Return the armor at that index
    return ArmorList[index];
}

Weapon GetRandomWeapon()
{
    if (WeaponList.empty()) {
        throw std::runtime_error("WeaponList is empty");
    }

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    int index = std::rand() % WeaponList.size();
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
