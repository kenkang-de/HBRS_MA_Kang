#include <algorithm>
#include <cmath>
#include <iostream>

#include "../Log/LogSystem.h"
#include "ActionLibrary.h"
#include "BoonAction.h"
#include "GlobalAction.h"
#include "TempBoonAction.h"
#include "Unit.h"

Unit::Unit()
    : Name(""), defaultStat(Stat()), totalStat(defaultStat), currentHP(0), weapon(nullptr), armor(nullptr),
      isFrozen(false), Tickinterval(1), TickDelay(0) {}

Unit::Unit(const Unit &other)
    : Name(other.Name), defaultStat(other.defaultStat), totalStat(other.totalStat), currentHP(other.currentHP),
      weapon(other.weapon), armor(other.armor), isFrozen(other.isFrozen), team(other.team),
      Tickinterval(other.Tickinterval), TickDelay(other.TickDelay) {
    // Don't copy activeBoons - start with empty vector
    // This is intentional: copied units start without active boons
}

// Custom assignment operator - don't copy activeBoons
Unit &Unit::operator=(const Unit &other) {
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

void Unit::ResetUnit() {
    totalStat = defaultStat;
    currentHP = GetDefaultStat().GetHP();
    weapon = nullptr;
    armor = nullptr;
    ClearActiveBoons();
    isFrozen = false;

    Tickinterval = 1;
    TickDelay = 0;
}

const std::string &Unit::GetName() const {
    return Name;
}

Stat &Unit::GetDefaultStat() {
    return defaultStat;
}

const Stat &Unit::GetDefaultStat() const {
    return defaultStat;
}

Stat &Unit::GetTotalStat() {
    return totalStat;
}

const Stat &Unit::GetTotalStat() const {
    return totalStat;
}

void Unit::SetWeapon(Weapon *w) {
    weapon = w;
    if (weapon) {
        // totalStat = weapon->GetStat();
        // for balancing
        if (weapon->correctionStat != nullptr)
            totalStat += *(weapon->correctionStat);
        else
            totalStat += weapon->GetStat();
    }
    currentHP = totalStat.GetHP();
}

void Unit::SetArmor(Armor *a) {
    armor = a;
    if (armor) {
        // totalStat = armor->GetStat();
        // for balancing
        if (armor->correctionStat != nullptr)
            totalStat += *(armor->correctionStat);
        else
            totalStat += armor->GetStat();
    }
    currentHP = totalStat.GetHP();
}

Weapon *Unit::GetWeapon() {
    return weapon;
}

Armor *Unit::GetArmor() {
    return armor;
}

// only used when it's need to update speed change dynamically during a battle.
void Unit::SetSpeed(int speed) {
    totalStat.SetSpeed(speed);
    TurnManager::UpdateSpeedChanges();
}

void Unit::TakeDamage(int amount, bool defendable, Unit *actor) {
    int finalDamage = amount;

    if (defendable) {
        finalDamage = std::max(0, amount - totalStat.GetDefense());
    } else {
        finalDamage = std::max(0, amount);
    }

    finalDamage = finalDamage * std::round(GetCounterMultiplier(actor, this));
    LogSystem::LogStream(Name, " HP: ", currentHP, " - ", finalDamage, " => ", currentHP - finalDamage);
    currentHP = std::max(0, currentHP - finalDamage);

    if (currentHP > 0)
        DelayRule::DelayUnitFromDamage(actor, this, finalDamage);
    else
        TurnManager::UpdateSpeedChanges();
}

void Unit::Heal(int amount) {
    int maxHP = totalStat.GetHP(); // Max HP from totalStat
    int oldHP = currentHP;
    currentHP = std::min(maxHP, currentHP + amount);
    int actualHealing = currentHP - oldHP;
    LogSystem::LogStream("[HEAL] ", Name, " healed for ", actualHealing, " HP (from ", oldHP, " to ", currentHP, "/",
                         maxHP, ")");
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

void Unit::AddBoon(std::unique_ptr<BoonAction> boon) {
    // Check if the same effect type already exists
    for (auto &existingBoon : activeBoons) {
        if (existingBoon->GetEffectType() == boon->GetEffectType()) {
            LogSystem::LogStream("[BOON] Refreshing existing  on ", Name);
            existingBoon->ResetUsage();
            return;
        }
    }
    // else add the boon to the list
    activeBoons.push_back(std::move(boon));
}

bool Unit::HasBoon(const std::string &effectType) const {
    for (const auto &boon : activeBoons) {
        if (boon->GetEffectType() == effectType && !boon->IsExpired()) {
            return true;
        }
    }
    return false;
}

void Unit::ApplyBoonsToAfterAction() {
    for (std::unique_ptr<BoonAction> &boon : activeBoons) {
        if (!boon->IsExpired()) {
            ActionContext context{boon->Caster, this};
            GlobalAction::AddAfterAction(boon.get(), context);
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

void Unit::ClearActiveBoons() {
    activeBoons.clear();
}
