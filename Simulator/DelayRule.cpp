#include "DelayRule.h"

void DelayRule::DelayUnitFromDamage(Unit *attacker, Unit *defender, int damage) {

    ActionType defenderType = defender->GetWeapon()->GetAction().GetActionType();

    if (defenderType == ActionType::MAGIC) {
        defender->TickDelay = defender->Tickinterval;
    }

    if (defenderType == ActionType::RANGE) {
        float damageRatio = static_cast<float>(damage) / static_cast<float>(defender->GetTotalStat().GetHP());
        float delayCalculation = damageRatio * DELAY_MULTIPLIER;
        defender->TickDelay = static_cast<int>(std::floor(delayCalculation));
    }
}