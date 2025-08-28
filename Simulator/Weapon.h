#ifndef WEAPON_H
#define WEAPON_H

#include "BattleAction.h"  
#include <string>
#include "Stat.h"

class Weapon {
    std::string id;
    std::string name;
    Stat stat;
    BattleAction action; 

public:
    Weapon(std::string id, std::string name, Stat stat, const BattleAction& action)
        : id(id), name(name), stat(stat), action(action) {}

    Weapon() : id(""), name("Default Weapon"), stat(Stat::Empty()), action() {}  

    const std::string& GetID() const { return id; }
    Stat& GetStat() { return stat; }
    const Stat& GetStat() const { return stat; }

    const BattleAction& GetAction() const { return action; }
};

#endif
