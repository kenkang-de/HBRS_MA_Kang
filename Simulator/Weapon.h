#ifndef WEAPON_H
#define WEAPON_H

#include "../Element/CounterTypeInitializer.h"
#include "../Element/Stat.h"
#include "BattleAction.h"
#include "TestSubject.h"
#include <string>

class Weapon : public TestSubject {
    std::string name;
    Stat stat;
    BattleAction action;

    CounterType weaponType;
    std::vector<std::string> unitSynergyIDs;

  public:
    Weapon(std::string id, std::string name, Stat stat, const BattleAction &action,
           CounterType _weaponType = CounterType::None)
        : TestSubject(id), name(name), stat(stat), action(action), weaponType(_weaponType) {}

    const std::string &GetName() const {
        return name;
    }
    Stat &GetStat() override {
        return stat;
    }
    const Stat &GetStat() const {
        return stat;
    }

    BattleAction &GetAction() {
        return action;
    }
    void SetWeaponType(CounterType _weaponType) {
        this->weaponType = _weaponType;
    }
    CounterType GetWeaponType() {
        return weaponType;
    }

    std::vector<std::string> &GetUnitSynergyIDs() {
        return unitSynergyIDs;
    }
};

#endif
