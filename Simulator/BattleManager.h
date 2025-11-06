#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include "../Paths.h"
#include "ActionLibrary.h"
#include "Battlefield.h"
#include "GlobalAction.h"
#include "TurnManager.h"
#include "Unit.h"
#include <vector>

class BattleManager {
  public:
    BattleManager(Battlefield &bf);
    ~BattleManager(); // Declaration only

    void StartBattle(bool log = false, std::string batchID = "NONE");

    void ProcessAfterActions(const std::vector<Unit *> &allUnits);

    // Boon management - now handled through Units
    void ApplyUnitBoonsToAfterAction(Unit *unit);

    // Public method to access TurnManager functionality
    void DelayUnit(Unit *unit, int delayAmount);

  private:
    Battlefield &battlefield;
    std::vector<Unit *> allUnits;
    TurnManager turnManager;

    int currentTick = 0;
    int CurrentTurn = 0;

    void ActUnits(std::vector<Unit *> &units, std::map<Unit *, std::vector<Unit *>> &allTargets);
    bool IsBattleOver();
    void LogDrawResult();
    void LogWinLoseResult();
    void LogUsageCount();
    int CalculateDelayFromDamage(int damageTaken, int maxHP);
};

#endif
