#ifndef DELAYRULE_H
#define DELAYRULE_H

#include <cmath>

#include "../Constants.h"
#include "BattleAction.h"
#include "Unit.h"

class DelayRule {
  public:
    static void DelayUnitFromDamage(Unit *attacker, Unit *defender, int damage);
};

#endif