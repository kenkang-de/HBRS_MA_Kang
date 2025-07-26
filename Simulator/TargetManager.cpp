#include "TargetManager.h"
#include <algorithm>

std::vector<Unit*> TargetManager::SelectTargets(
    Unit* actingUnit,
    const std::vector<Unit*>& allies,
    const std::vector<Unit*>& enemies,
    const BattleAction& action)
{
    std::vector<Unit*> candidates;

    // Determine target pool
    if (action.GetTargetType() == TargetType::ALLY) {
        candidates = allies;
    } else {
        candidates = enemies;
    }

    // Exclude dead units
    candidates.erase(
        std::remove_if(candidates.begin(), candidates.end(), [](Unit* u) {
            return u->GetTotalStat().GetHP() <= 0;
        }),
        candidates.end()
    );

    // Handle self-targeting
    if (!action.IncludesSelf()) {
        candidates.erase(
            std::remove(candidates.begin(), candidates.end(), actingUnit),
            candidates.end()
        );
    }

    // Sort by threat descending
    std::sort(candidates.begin(), candidates.end(), [](Unit* a, Unit* b) {
        return a->GetTotalStat().GetThreat() > b->GetTotalStat().GetThreat();
    });

    // Limit to targetNumber
    int maxTargets = action.GetTargetNumber();
    if (maxTargets > 0 && candidates.size() > maxTargets) {
        candidates.resize(maxTargets);
    }

    return candidates;
}
