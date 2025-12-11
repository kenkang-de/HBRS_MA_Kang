#include "../Constants.h"
#include "../Log/LogSystem.h"
#include "Unit.h"

// target must be enemy
float GetCounterMultiplier(Unit *actor, Unit *target) {
    CounterType weaponType = actor->GetWeapon()->GetWeaponType();
    CounterType armorType = target->GetArmor()->GetArmorType();

    // Rock beats Scissor
    if (weaponType == CounterType::Rock) {
        if (armorType == CounterType::Scissor) {
            LogSystem::LogStream("COUNTER APPLIED");
            return MULTIPLIER_COUNTER;
        }

        else
            return MULTIPLIER_BASIC;
    }
    // Paper beats Rock
    else if (weaponType == CounterType::Paper) {
        if (armorType == CounterType::Rock) {
            LogSystem::LogStream("COUNTER APPLIED");
            return MULTIPLIER_COUNTER;
        } else
            return MULTIPLIER_BASIC;
    }
    // Scissor beats Paper
    else if (weaponType == CounterType::Scissor) {
        if (armorType == CounterType::Paper) {
            LogSystem::LogStream("COUNTER APPLIED");
            return MULTIPLIER_COUNTER;
        } else
            return MULTIPLIER_BASIC;
    }
    // Default case (None or unknown types)
    else {
        return MULTIPLIER_BASIC;
    }
}

bool IsConunterRelated(Unit *actor, Unit *target) {
    CounterType weaponType = actor->GetWeapon()->GetWeaponType();
    CounterType armorType = target->GetArmor()->GetArmorType();

    // Rock beats Scissor
    if (weaponType == CounterType::Rock) {
        if (armorType == CounterType::Scissor) {
            LogSystem::LogStream("COUNTER APPLIED");
            return true;
        }

        else
            return false;
    }
    // Paper beats Rock
    else if (weaponType == CounterType::Paper) {
        if (armorType == CounterType::Rock) {
            LogSystem::LogStream("COUNTER APPLIED");
            return true;
        } else
            return false;
    }
    // Scissor beats Paper
    else if (weaponType == CounterType::Scissor) {
        if (armorType == CounterType::Paper) {
            LogSystem::LogStream("COUNTER APPLIED");
            return true;
        } else
            return false;
    }
    // Default case (None or unknown types)
    else {
        return false;
    }
}