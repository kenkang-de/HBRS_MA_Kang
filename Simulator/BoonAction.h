#ifndef BOONACTION_H
#define BOONACTION_H

#include "BattleAction.h"
#include <string>

class Unit;

class BoonAction : public BattleAction {
private:
    mutable int usageNumber; // How many times this boon can trigger (mutable for const Perform)
    int maxUsage;            // Original usage for reference
    std::string effectType;  // Type of effect (buff/debuff identifier)
    
public:
    BoonAction();
    BoonAction(const std::string& id, const std::string& effectType, int usage);
    
    // Getters
    int GetUsageNumber() const { return usageNumber; }
    int GetMaxUsage() const { return maxUsage; }
    const std::string& GetEffectType() const { return effectType; }
    
    // Usage management
    void DecrementUsage() { if (usageNumber > 0) usageNumber--; }
    bool IsExpired() const { return usageNumber <= 0; }
    
    // Reset usage (for reapplying same effect)
    void ResetUsage() { usageNumber = maxUsage; }
    
    // Override Perform to handle boon-specific logic
    void Perform(Unit* actor, Unit* target, const std::vector<Unit*>& allies, const std::vector<Unit*>& enemies) const override;
};

#endif
