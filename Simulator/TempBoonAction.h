#ifndef TEMPBOONACTION_H
#define TEMPBOONACTION_H

#include "BoonAction.h"

class TempBoonAction : public BoonAction {
  private:
    mutable bool hasBeenApplied = false;
    mutable bool effectExecuted = false;
    std::string removalEffectName;

  public:
    TempBoonAction(std::string id, std::string effectType, int duration, std::string removalEffect, Unit *caster);

    // Getters
    bool HasBeenApplied() const {
        return hasBeenApplied;
    }
    bool HasEffectExecuted() const {
        return effectExecuted;
    }
    const std::string &GetRemovalEffectName() const {
        return removalEffectName;
    }

    // Setters
    void SetRemovalEffectName(const std::string &removalEffect) {
        removalEffectName = removalEffect;
    }

    // Mark as applied
    void MarkAsApplied() const {
        hasBeenApplied = true;
    }
    void MarkEffectExecuted() const {
        effectExecuted = true;
    }

    void ResetUsage() override {
        BoonAction::ResetUsage();
    }

    // Override Perform to handle temporary buff logic
    void Perform(Unit *actor, Unit *target) override;
};

#endif
