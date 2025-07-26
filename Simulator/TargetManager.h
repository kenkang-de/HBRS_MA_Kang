#ifndef TARGET_MANAGER_H
#define TARGET_MANAGER_H

#include <vector>
#include "Unit.h"
#include "BattleAction.h"

class TargetManager {
public:
    static std::vector<Unit*> SelectTargets(
        Unit* actingUnit,
        const std::vector<Unit*>& allies,
        const std::vector<Unit*>& enemies,
        const BattleAction& action);
};

#endif
