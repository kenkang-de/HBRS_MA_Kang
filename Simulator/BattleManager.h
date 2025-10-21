#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include "Unit.h"
#include "TurnManager.h"
#include "Battlefield.h"
#include "ActionLibrary.h"
#include "GlobalAction.h"
#include "../Paths.h"
#include <vector>

class BattleManager {
public:
    BattleManager(Battlefield& bf);
    ~BattleManager(); // Declaration only

    void StartBattle(bool log=false, std::string batchID="NONE");
    
    void ProcessAfterActions(const std::vector<Unit*>& allUnits);
    
    // Boon management - now handled through Units
    void ApplyUnitBoonsToAfterAction(Unit* unit);
    
    // Public method to access TurnManager functionality
    void DelayUnit(Unit* unit, int delayAmount);

private:
    Battlefield& battlefield;
    std::vector<Unit*> allUnits;
    TurnManager turnManager;

    int currentTick = 0; 
    int CurrentTurn = 0;
    int tickCount = 0;  // Counter for number of ticks that have occurred

    bool IsBattleOver(bool test,int tickCount);
    void LogDrawResult();
    void LogWinLoseResult();
    void LogUsageCount();
    int CalculateDelayFromDamage(int damageTaken, int maxHP);

void SplitAlliesAndEnemies(Unit* unit, const BattleAction& action, std::vector<Unit*>& allies, std::vector<Unit*>& enemies);

};

#endif
