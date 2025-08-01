#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include "Unit.h"
#include "TurnManager.h"
#include "Battlefield.h"
#include <vector>

class BattleAction;

class BattleManager {
public:
    BattleManager(Battlefield& bf);

    void StartBattle();

private:
    Battlefield& battlefield;
    std::vector<Unit*> allUnits;
    TurnManager turnManager;

    int currentTick = 0; 
    int CurrentTurn = 0;

    void PerformAction(Unit* unit);
    bool IsBattleOver(bool test);

void SplitAlliesAndEnemies(Unit* unit, const BattleAction& action, std::vector<Unit*>& allies, std::vector<Unit*>& enemies);

};

#endif
