#include "TargetManager.h"
#include <algorithm>
#include <limits.h>

std::vector<Unit *> TargetManager::GetTargets(Unit &actor) {

    std::vector<Unit *> targets;

    // get action info
    TargetType targetType = actor.GetWeapon()->GetAction().GetTargetType();
    int targetNum = actor.GetWeapon()->GetAction().GetTargetNumber();
    bool self = actor.GetWeapon()->GetAction().IncludesSelf();

    targets = targetType == TargetType::ENEMY ? actor.Enemies : actor.Allies;

    targets.erase(std::remove_if(targets.begin(), targets.end(),
                                 [&actor, self, targetType](Unit *target) {
                                     // Remove if dead
                                     if (!target->IsAlive()) {
                                         return true;
                                     }

                                     // Remove self if targeting allies and self is not allowed
                                     if (targetType == TargetType::ALLY && target == &actor && !self) {
                                         return true;
                                     }

                                     return false;
                                 }),
                  targets.end());

    targetNum = std::min(static_cast<int>(targets.size()), targetNum);

    return FindHighestThreatTargets(targets, targetNum);
}

std::vector<Unit *> TargetManager::GetTargets(Unit &actor, const BattleAction &action) {

    std::vector<Unit *> targets;

    // get action info
    TargetType targetType = action.GetTargetType();
    int targetNum = action.GetTargetNumber();
    bool self = action.IncludesSelf();

    targets = targetType == TargetType::ENEMY ? actor.Enemies : actor.Allies;

    targets.erase(std::remove_if(targets.begin(), targets.end(),
                                 [&actor, self, targetType](Unit *target) {
                                     // Remove if dead
                                     if (!target->IsAlive()) {
                                         return true;
                                     }

                                     // Remove self if targeting allies and self is not allowed
                                     if (targetType == TargetType::ALLY && target == &actor && !self) {
                                         return true;
                                     }

                                     return false;
                                 }),
                  targets.end());

    targetNum = std::min(static_cast<int>(targets.size()), targetNum);

    return FindHighestThreatTargets(targets, targetNum);
}

std::vector<Unit *> TargetManager::FindHighestThreatTargets(const std::vector<Unit *> &candidates, int count) {
    std::vector<Unit *> validCandidates = candidates;

    // Sort by threat value (highest first)
    std::sort(validCandidates.begin(), validCandidates.end(),
              [](Unit *a, Unit *b) { return a->GetTotalStat().GetThreat() > b->GetTotalStat().GetThreat(); });

    std::vector<Unit *> result;

    for (int i = 0; i < count; i++) {
        result.push_back(validCandidates[i]);
    }

    return result;
}
