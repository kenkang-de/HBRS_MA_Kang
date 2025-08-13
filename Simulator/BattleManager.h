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
    std::string actionID;  // Alternative: store action ID for dynamic creation
    ActionContext context;
    
    // Constructor for BattleAction pointer
    AfterActionEvent(const BattleAction* action, const ActionContext& ctx)
        : battleAction(action), actionID(""), context(ctx) {}
        
    // Constructor for action ID
    AfterActionEvent(const std::string& id, const ActionContext& ctx)
        : battleAction(nullptr), actionID(id), context(ctx) {}
};

class BattleManager {
public:
    BattleManager(Battlefield& bf);

    void StartBattle();
    
    // Static methods for after-action system
    static void AddAfterAction(const BattleAction* battleAction, const ActionContext& context);
    static void AddAfterAction(const std::string& actionID, const ActionContext& context);
    void ProcessAfterActions(const std::vector<Unit*>& allUnits);

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
