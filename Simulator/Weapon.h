#ifndef WEAPON_H
#define WEAPON_H

#include "BattleAction.h"  
#include <string>
#include "../Element/Stat.h"
#include "TestSubject.h"

class Weapon : public TestSubject {
    std::string name;
    Stat stat;
    BattleAction action; 

public:
    Weapon(std::string id, std::string name, Stat stat, const BattleAction& action)
        : TestSubject(id), name(name), stat(stat), action(action) {}

    const std::string& GetName() const { return name; }
    Stat& GetStat() { return stat; }
    const Stat& GetStat() const { return stat; }

    const BattleAction& GetAction() const { return action; }
};

#endif
