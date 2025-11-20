#ifndef BOONACTION_H
#define BOONACTION_H

#include "BattleAction.h"
#include <string>

class Unit;

class BoonAction : public BattleAction {
  private:
    mutable int usageNumber;
    int maxUsage;
    std::string effectType;

  public:
    BoonAction(const std::string &id, const std::string &effectType, int usage, Unit *caster)
        : BattleAction(), effectType(effectType), usageNumber(usage), maxUsage(usage), Caster(caster) {
        SetID(id);
    }

    Unit *Caster;

    int GetUsageNumber() const {
        return usageNumber;
    }
    int GetMaxUsage() const {
        return maxUsage;
    }
    const std::string &GetEffectType() const {
        return effectType;
    }

    void DecrementUsage() {
        if (usageNumber > 0)
            usageNumber--;
    }
    bool IsExpired() const {
        return usageNumber <= 0;
    }

    virtual void ResetUsage() {
        usageNumber = maxUsage;
    }
    void Perform(Unit *actor, Unit *target) override;
};

#endif
