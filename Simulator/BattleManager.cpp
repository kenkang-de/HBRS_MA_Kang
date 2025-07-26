#include <iostream>


#include "BattleManager.h"
#include "Constants.h"
#include "TargetManager.h" 


BattleManager::BattleManager(std::vector<Unit*>& teamRed, std::vector<Unit*>& teamBlue) {
    allUnits.reserve(teamRed.size() + teamBlue.size());
    allUnits.insert(allUnits.end(), teamRed.begin(), teamRed.end());
    allUnits.insert(allUnits.end(), teamBlue.begin(), teamBlue.end());
}

void BattleManager::StartBattle() {
    turnManager.Initialize(allUnits);

    while (!IsBattleOver(true)) {
    while (Unit* unit = turnManager.GetNextUnit()) {
        // std::cout << "[Tick " << currentTick << "] " << unit->Name << " is acting.\n";
        PerformAction(unit);
    }

    turnManager.AdvanceTick();
    ++currentTick;
}

    std::cout << "Battle finished.\n";
}



void BattleManager::SplitAlliesAndEnemies(Unit* unit, const BattleAction& action, std::vector<Unit*>& allies, std::vector<Unit*>& enemies) {
    for (Unit* u : allUnits) {
        if (u->GetTotalStat().GetHP() <= 0) continue;

        if (u->team == unit->team) {
            if (u == unit && !action.IncludesSelf()) continue;  // skip self if action does not include self
            allies.push_back(u);
        } else {
            enemies.push_back(u);
        }
    }
}




void BattleManager::PerformAction(Unit* unit) {
    const BattleAction& action = unit->GetWeapon().GetAction();

    std::vector<Unit*> allies, enemies;
    SplitAlliesAndEnemies(unit, action, allies, enemies);

    std::cout << "[Tick " << currentTick << "] " << unit->Name 
              << " (" << (unit->team == Red ? "Red" : "Blue") << ") performs "
              << action.GetID() << "\n";

    action.Perform(unit, allies, enemies);
}



bool BattleManager::IsBattleOver(bool test) {

    if(test)
    {
        return currentTick > TEST_ROUND;
    }

    //not for testing, TODO:: when one of all team members hitpoint reaches below 0.
    else
    {
    int aliveCount = 0;
    for (Unit* u : allUnits) {
        if (u->GetTotalStat().GetHP() > 0)
            aliveCount++;
    }
    return aliveCount <= 1; // Only 1 unit standing
    }

    return true;

}
