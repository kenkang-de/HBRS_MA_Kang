#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "Stat.h"
#include "Weapon.h"
#include "Armor.h"
#include "Team.h"

class Unit {
private:

    Stat defaultStat;
    Stat totalStat;

    Weapon weapon;
    Armor armor;

public:
    std::string Name;
    std::string ID;
    TeamColor team;

Unit(std::string name, std::string id);

    const std::string& GetName() const;

    Stat& GetDefaultStat();
    const Stat& GetDefaultStat() const;

    Stat& GetTotalStat();
    const Stat& GetTotalStat() const;

    void SetWeapon(const Weapon& w);
    void SetArmor(const Armor& a);

    void UnEquipWeapon();
    void UnEquipArmor();

    Weapon& GetWeapon();
    const Weapon& GetWeapon() const;

    Armor& GetArmor();
    const Armor& GetArmor() const;

    void TakeDamage(int amount);
void Heal(int amount);

void ApplyBuff();     // Placeholder
void ApplyDebuff();   // Placeholder

void TakeDamage(int amount, bool defendable = true);
};

#endif
