#include "Unit.h"
#include <iostream>

Unit::Unit(std::string name, std::string id)
    : Name(name), ID(id), 
      defaultStat(Stat()),
      totalStat(defaultStat),
      currentHP(0) // Initialize currentHP to 0, will be set when equipment is applied
       {}

const std::string& Unit::GetName() const {
    return Name;
}

Stat& Unit::GetDefaultStat() {
    return defaultStat;
}

const Stat& Unit::GetDefaultStat() const {
    return defaultStat;
}
 
Stat& Unit::GetTotalStat() {
    return totalStat;
}

const Stat& Unit::GetTotalStat() const {
    return totalStat;
}

void Unit::SetWeapon(const Weapon& w) {
    // Subtract old weapon stats
    totalStat -= weapon.GetStat();
    weapon = w;
    // Add new weapon stats
    totalStat += weapon.GetStat();
    // Update current HP to match new total HP
    currentHP = totalStat.GetHP();
}

void Unit::SetArmor(const Armor& a) {
    totalStat -= armor.GetStat();
    armor = a;
    totalStat += armor.GetStat();
    // Update current HP to match new total HP
    currentHP = totalStat.GetHP();
}

void Unit::UnEquipWeapon() {
    totalStat -= weapon.GetStat();
    weapon = Weapon(); // reset to default
}

void Unit::UnEquipArmor() {
    totalStat -= armor.GetStat();
    armor = Armor(); // reset to default
}

Weapon& Unit::GetWeapon() {
    return weapon;
}

const Weapon& Unit::GetWeapon() const {
    return weapon;
}

Armor& Unit::GetArmor() {
    return armor;
}

const Armor& Unit::GetArmor() const {
    return armor;
}

void Unit::TakeDamage(int amount, bool defendable) {
    int finalDamage = amount;
    if (defendable) {
        finalDamage = std::max(0, amount - totalStat.GetDefense());
    }

    // Subtract damage from current HP
    currentHP = std::max(0, currentHP - finalDamage);
    std::cout << Name << " took damage: " << finalDamage << (defendable ? " (defended)\n" : " (pierced)\n");
    std::cout << Name << "'s Remaining Health: " << currentHP << std::endl;
}


void Unit::Heal(int amount) {
    int maxHP = totalStat.GetHP();  // Max HP from totalStat
    currentHP = std::min(maxHP, currentHP + amount);
}

void Unit::ApplyBuff() {
    // Example: boost speed by 1
    totalStat.SetSpeed(totalStat.GetSpeed() + 1);
}

void Unit::ApplyDebuff() {
    // Example: reduce defense by 1, not below 0
    int newDef = std::max(0, totalStat.GetDefense() - 1);
    totalStat.SetDefense(newDef);
}
