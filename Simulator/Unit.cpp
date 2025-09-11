#include "Unit.h"
#include "BoonAction.h"
#include "TempBoonAction.h"
#include "ActionLibrary.h"
#include <iostream>
#include <algorithm>

// Forward declaration for global function
void AddAfterActionToBattleManager(const class BattleAction* action, const struct ActionContext& context);

Unit::Unit(std::string name, std::string id)
    : Name(name), ID(id), 
      defaultStat(Stat()),
      totalStat(defaultStat),
      currentHP(0), // Initialize currentHP to 0, will be set when equipment is applied
      weapon(nullptr),
      armor(nullptr),
      isFrozen(false) // Initialize isFrozen to false by default
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

void Unit::SetWeapon(const Weapon* w) {
    if (weapon) {
        totalStat -= weapon->GetStat();
    }
    weapon = w;
    if (weapon) {
        totalStat += weapon->GetStat();
    }
    currentHP = totalStat.GetHP();
}

void Unit::SetArmor(const Armor* a) {
    if (armor) {
        totalStat -= armor->GetStat();
    }
    armor = a;
    if (armor) {
        totalStat += armor->GetStat();
    }
    currentHP = totalStat.GetHP();
}

const Weapon* Unit::GetWeapon() const {
    return weapon;
}

const Armor* Unit::GetArmor() const {
    return armor;
}

void Unit::TakeDamage(int amount, bool defendable) {
    int finalDamage = amount;
    
    if (defendable) {
        finalDamage = std::max(0, amount - totalStat.GetDefense());
    }

    currentHP = std::max(0, currentHP - finalDamage);
    std::cout << Name << " HP: "<< currentHP + finalDamage << " - " << finalDamage << " => " << currentHP << std::endl;
}


void Unit::Heal(int amount) {
    int maxHP = totalStat.GetHP();  // Max HP from totalStat
    int oldHP = currentHP;
    currentHP = std::min(maxHP, currentHP + amount);
    int actualHealing = currentHP - oldHP;
    std::cout << "[HEAL] " << Name << " healed for " << actualHealing << " HP (from " << oldHP << " to " << currentHP << "/" << maxHP << ")" << std::endl;
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
            std::cout << "[BOON] Refreshing existing " << boon->GetEffectType() 
                      << " on " << Name << std::endl;
            
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
                    std::cout << "[REFRESH] " << boon->GetEffectType() << " duration refreshed, effect remains active" << std::endl;
                } else {
                    std::cout << "[REFRESH] " << boon->GetEffectType() << " duration refreshed, ready for first effect" << std::endl;
                }
            } else {
                // For regular boons, normal reset
                existingBoon->ResetUsage();
            }
            return;
        }
    }
    
    // Add new boon
    std::cout << "[BOON] Applied " << boon->GetEffectType() 
              << " to " << Name << " (Usage: " << boon->GetUsageNumber() << ")" << std::endl;
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
        std::cout << "[DEBUG] " << Name << " processing " << activeBoons.size() << " boons" << std::endl;
    }
    for (auto& boon : activeBoons) {
        if (!boon->IsExpired()) {
            std::cout << "[DEBUG] Registering boon " << boon->GetEffectType() << " for " << Name << std::endl;
            // Register this boon in the after-action system
            AddAfterActionToBattleManager(boon.get(), {this, this, {}, {}});
        } else {
            std::cout << "[DEBUG] Skipping expired boon " << boon->GetEffectType() << " for " << Name << std::endl;
        }
    }
}

void Unit::CleanupExpiredBoons() {
    auto it = activeBoons.begin();
    while (it != activeBoons.end()) {
        if ((*it)->IsExpired()) {
            std::cout << "[BOON] " << (*it)->GetEffectType() 
                      << " expired on " << Name << " - cleaned up" << std::endl;
            
            // Note: TempBoonAction handles its own removal effects in Perform()
            // so we don't need to execute removal effects here to avoid double execution
            
            it = activeBoons.erase(it);
        } else {
            ++it;
        }
    }
}

