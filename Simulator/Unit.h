#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <memory>

#include "../Element/Stat.h"
#include "Weapon.h"
#include "Armor.h"
#include "Team.h"
#include "BoonAction.h"

class Unit {
private:

    Stat defaultStat;
    Stat totalStat;
    int currentHP; // Current health points, separate from totalStat

    Weapon* weapon;
    Armor* armor;
    
    // Boon management
    std::vector<std::unique_ptr<BoonAction>> activeBoons;

    bool isFrozen;

public:
    std::string Name;
    TeamColor team;

    int Tickinterval;
    int TickDelay;

    Unit();
    
    // Custom copy constructor and assignment operator to handle unique_ptr
    Unit(const Unit& other);
    Unit& operator=(const Unit& other);
    
    // Default move constructor and move assignment operator
    Unit(Unit&&) = default;
    Unit& operator=(Unit&&) = default;

    void ResetUnit();

    const std::string& GetName() const;

    Stat& GetDefaultStat();
    const Stat& GetDefaultStat() const;

    Stat& GetTotalStat();
    const Stat& GetTotalStat() const;

    void SetWeapon(Weapon* w);
    void SetArmor(Armor* a);

    Weapon* GetWeapon();
    const Weapon* GetWeapon() const;
    Armor* GetArmor();
    const Armor* GetArmor() const;

    void TakeDamage(int amount);
    void Heal(int amount);
    int GetCurrentHP() const { return currentHP; }
    void SetCurrentHP(int hp) { currentHP = hp; }
    void InitializeCurrentHP() { currentHP = totalStat.GetHP(); } // Initialize currentHP to match totalStat HP

    void ApplyBuff();     // Placeholder
    void ApplyDebuff();   // Placeholder

    void TakeDamage(int amount, bool defendable = true);

    bool IsAlive() const { return currentHP > 0; };
    bool IsFrozen() const { return isFrozen; }
    void SetFrozen(bool frozen) { isFrozen = frozen; }
    void EnhanceHP(int amount);
    
    // Boon management methods
    void AddBoon(std::unique_ptr<BoonAction> boon);
    bool HasBoon(const std::string& effectType) const;
    void ApplyBoonsToAfterAction(); // Called by BattleManager to register boons in after-actions
    void CleanupExpiredBoons();     // Remove boons with 0 usage
    void ClearActiveBoons();
};

#endif
