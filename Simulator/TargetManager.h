#ifndef TARGET_MANAGER_H
#define TARGET_MANAGER_H

#include "BattleAction.h"
#include "Unit.h"
#include <map>
#include <vector>

class TargetManager {
  public:
    static std::vector<Unit *> GetTargets(Unit &actor);
    static std::vector<Unit *> GetTargets(Unit &actor, const BattleAction &action);

  private:
    static std::vector<Unit *> FindHighestThreatTargets(const std::vector<Unit *> &candidates, int count);
};

#endif
