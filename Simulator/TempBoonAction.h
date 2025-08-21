#ifndef TEMPBOONACTION_H
#define TEMPBOONACTION_H

#include "BoonAction.h"

class TempBoonAction : public BoonAction {
private:
    mutable bool hasBeenApplied;    // Track if the buff has been applied
    std::string removalEffectName;  // Name of the removal effect to apply when expired
    
public:
    TempBoonAction();
    TempBoonAction(const std::string& id, const std::string& effectType, int duration, const std::string& removalEffect = "");
    
    // Getters
    bool HasBeenApplied() const { return hasBeenApplied; }
    const std::string& GetRemovalEffectName() const { return removalEffectName; }
    
    // Setters
    void SetRemovalEffectName(const std::string& removalEffect) { removalEffectName = removalEffect; }
    
    // Mark as applied
    void MarkAsApplied() const { hasBeenApplied = true; }
    
    // Reset for reapplication
    void ResetUsage() override { 
        BoonAction::ResetUsage(); 
        hasBeenApplied = false; 
    }
    
    // Override Perform to handle temporary buff logic
    void Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const override;
};

#endif
