#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include "Unit.h"
#include "TurnManager.h"
#include "Battlefield.h"
#include "ActionLibrary.h"
#include <vector>

class BattleAction;

// Structure to hold after-action events
struct AfterActionEvent {
    const BattleAction* battleAction;
    ActionContext context;
    
    AfterActionEvent(const BattleAction* action, const ActionContext& ctx)
        : battleAction(action), context(ctx) {}
};

class BattleManager {
public:
    BattleManager(Battlefield& bf);

    void StartBattle();
    
    // Static methods for after-action system
    static void AddAfterAction(const BattleAction* battleAction, const ActionContext& context);
    void ProcessAfterActions(const std::vector<Unit*>& allUnits);
    
    // Boon management - now handled through Units
    void ApplyUnitBoonsToAfterAction(Unit* unit);
    
    // Public method to access TurnManager functionality
    void DelayUnit(Unit* unit, int delayAmount);
    int GetUnitInterval(Unit* unit) const;

private:
    Battlefield& battlefield;
    std::vector<Unit*> allUnits;
    TurnManager turnManager;
    
    // Static vector for after-action events
    static std::vector<AfterActionEvent> afterActions;

    int currentTick = 0; 
    int CurrentTurn = 0;

    bool IsBattleOver(bool test);
    int CalculateDelayFromDamage(int damageTaken, int maxHP);

void SplitAlliesAndEnemies(Unit* unit, const BattleAction& action, std::vector<Unit*>& allies, std::vector<Unit*>& enemies);

};

#endif
