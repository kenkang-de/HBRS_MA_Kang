
#include <list>
#include <iostream>
#include "Unit.h"
#include "UnitGenerator.h"
#include "TurnManager.h"
#include "EquipmentManager.h"
#include "Team.h"
#include "Battlefield.h"
#include "BattleActionLoader.h"
#include "BattleManager.h" 


int main()
{
    std::list<Unit> allUnits = GenerateUnits(10);

    auto actionMap = LoadActionsFromYAML("BattleActions.yaml");

    LoadArmorListFromCSV("SpeedTest_A.csv");
    LoadWeaponListFromCSV("MeleeRangeTest.csv",actionMap);

    EquipUnitsRandomEquipments(allUnits);

    // Create stable pointers before any list operations
    std::vector<Unit*> allUnitPtrs;
    for (Unit& unit : allUnits) {
        allUnitPtrs.push_back(&unit);
    }

    // Split into two teams using pointers instead of splice
    std::vector<Unit*> redPtrs(allUnitPtrs.begin(), allUnitPtrs.begin() + 5);
    std::vector<Unit*> bluePtrs(allUnitPtrs.begin() + 5, allUnitPtrs.end());

    // Don't use Team class for now, work directly with Unit pointers
    BattleManager battleManager(redPtrs, bluePtrs);
    battleManager.StartBattle();
    
    std::cin.get(); // Waits for Enter key
    return 0;
}