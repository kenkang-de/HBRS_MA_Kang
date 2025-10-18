#include <iostream>
#include <algorithm>
#include <cmath>

#include "Unit.h"
#include "BoonAction.h"
#include "TempBoonAction.h"
#include "ActionLibrary.h"
#include "GlobalAction.h"
#include "../Log/LogSystem.h"


Unit::Unit()
    : Name(""),
      defaultStat(Stat()),
      totalStat(defaultStat),
      // Initialize currentHP to 0, will be set when equipment is applied
      currentHP(0), 
      weapon(nullptr),
      armor(nullptr),
      isFrozen(false), // Initialize isFrozen to false by default
      Tickinterval(0),
      TickDelay(0)
       {}

// Custom copy constructor - don't copy activeBoons (start fresh)
Unit::Unit(const Unit& other)
    : Name(other.Name),
      defaultStat(other.defaultStat),
      totalStat(other.totalStat),
      currentHP(other.currentHP),
      weapon(other.weapon),
      armor(other.armor),
      isFrozen(other.isFrozen),
      team(other.team),
      Tickinterval(other.Tickinterval),
      TickDelay(other.TickDelay)
{
    // Don't copy activeBoons - start with empty vector
    // This is intentional: copied units start without active boons
}

// Custom assignment operator - don't copy activeBoons
Unit& Unit::operator=(const Unit& other) {
    if (this != &other) {
        Name = other.Name;
        defaultStat = other.defaultStat;
        totalStat = other.totalStat;
        currentHP = other.currentHP;
        weapon = other.weapon;
        armor = other.armor;
        isFrozen = other.isFrozen;
        team = other.team;
        Tickinterval = other.Tickinterval;
        TickDelay = other.TickDelay;
        
        // Clear existing boons and don't copy from other
        activeBoons.clear();
    }
    return *this;
}

void Unit::ResetUnit()
{
    totalStat = defaultStat; 
    currentHP = GetDefaultStat().GetHP();
    weapon = nullptr;
    armor = nullptr;
    ClearActiveBoons();
    isFrozen = false;

    Tickinterval=0;
    TickDelay=0;
}

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

void Unit::SetWeapon(Weapon* w) {
    weapon = w;
    if (weapon) {
        totalStat += weapon->GetStat();
        //for balancing
        if(weapon->correctionStat != nullptr)
            totalStat += *(weapon->correctionStat);
    }
    currentHP = totalStat.GetHP();
}

void Unit::SetArmor(Armor* a) {
    armor = a;
    if (armor) {
        totalStat += armor->GetStat();
        //for balancing
        if(armor->correctionStat != nullptr)
        totalStat += *(armor->correctionStat);
    }
    currentHP = totalStat.GetHP();
}

Weapon* Unit::GetWeapon() {
    return weapon;
}

const Weapon* Unit::GetWeapon() const {
    return weapon;
}

Armor* Unit::GetArmor() {
    return armor;
}

const Armor* Unit::GetArmor() const {
    return armor;
}

void Unit::TakeDamage(int amount, bool defendable, Unit* actor) {
    int finalDamage = amount;

    if (defendable) {
        finalDamage = std::max(0, amount - totalStat.GetDefense());
    }

    finalDamage = finalDamage * std::round(GetCounterMultiplier(actor, this));

    currentHP = std::max(0, currentHP - finalDamage);
    LogSystem::LogStream(Name, " HP: ", currentHP + finalDamage, " - ", finalDamage, " => ", currentHP);
}


void Unit::Heal(int amount) {
    int maxHP = totalStat.GetHP();  // Max HP from totalStat
    int oldHP = currentHP;
    currentHP = std::min(maxHP, currentHP + amount);
    int actualHealing = currentHP - oldHP;
    LogSystem::LogStream("[HEAL] ", Name, " healed for ", actualHealing, " HP (from ", oldHP, " to ", currentHP, "/", maxHP, ")");
}

void Unit::ApplyDebuff() {
    // Example: reduce defense by 1, not below 0
    int newDef = std::max(0, totalStat.GetDefense() - 1);
    totalStat.SetDefense(newDef);
}

void Unit::EnhanceHP(int amount) {
    int newHP = std::max(0, totalStat.GetHP() + amount);
    totalStat.SetHP(newHP);
    currentHP += amount; 
}

// Boon management methods
void Unit::AddBoon(std::unique_ptr<BoonAction> boon) {
    // Check if the same effect type already exists
    for (auto& existingBoon : activeBoons) {
        if (existingBoon->GetEffectType() == boon->GetEffectType()) {
            LogSystem::LogStream("[BOON] Refreshing existing ", boon->GetEffectType(), " on ", Name);
            
            // For TempBoonAction, we want to reset duration but NOT reapply the effect
            TempBoonAction* tempBoon = dynamic_cast<TempBoonAction*>(existingBoon.get());
            if (tempBoon) {
                // Store the current execution state before reset
                bool wasExecuted = tempBoon->HasEffectExecuted();
                
                // Reset the usage counter (this also resets flags)
                BoonAction* baseBoon = static_cast<BoonAction*>(tempBoon);
                baseBoon->ResetUsage();  
                
                // If effect was already executed, prevent reapplication
                if (wasExecuted) {
                    tempBoon->MarkAsApplied();
                    tempBoon->MarkEffectExecuted();
                    LogSystem::LogStream("[REFRESH] ", boon->GetEffectType(), " duration refreshed, effect remains active");
                } else {
                    LogSystem::LogStream("[REFRESH] ", boon->GetEffectType(), " duration refreshed, ready for first effect");
                }
            } else {
                // For regular boons, normal reset
                existingBoon->ResetUsage();
            }
            return;
        }
    }
    
    // Add new boon
    LogSystem::LogStream("[BOON] Applied ", boon->GetEffectType(), " to ", Name, " (Usage: ", boon->GetUsageNumber(), ")");
    activeBoons.push_back(std::move(boon));
}

bool Unit::HasBoon(const std::string& effectType) const {
    for (const auto& boon : activeBoons) {
        if (boon->GetEffectType() == effectType && !boon->IsExpired()) {
            return true;
        }
    }
    return false;
}

void Unit::ApplyBoonsToAfterAction() {
    // This will be called by BattleManager to register active boons as after-actions
    if (!activeBoons.empty()) {
        LogSystem::LogStream("[DEBUG] ", Name, " processing ", activeBoons.size(), " boons");
    }
    for (auto& boon : activeBoons) {
        if (!boon->IsExpired()) {
            LogSystem::LogStream("[DEBUG] Registering boon ", boon->GetEffectType(), " for ", Name);
            // Register this boon in the after-action system
            GlobalAction::AddAfterAction(boon.get(), {this, this, {}, {}});
        } else {
            LogSystem::LogStream("[DEBUG] Skipping expired boon ", boon->GetEffectType(), " for ", Name);
        }
    }
}

void Unit::CleanupExpiredBoons() {
    auto it = activeBoons.begin();
    while (it != activeBoons.end()) {
        if ((*it)->IsExpired()) {
            LogSystem::LogStream("[BOON] ", (*it)->GetEffectType(), " expired on ", Name, " - cleaned up");
            
            // Note: TempBoonAction handles its own removal effects in Perform()
            // so we don't need to execute removal effects here to avoid double execution
            
            it = activeBoons.erase(it);
        } else {
            ++it;
        }
    }
}

void Unit::ClearActiveBoons()
{
    activeBoons.clear();
}

