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

    LoadArmorListFromCSV("Data/Test/NoArmor.csv");
    LoadWeaponListFromCSV("Data/Test/SmokeShotTest.csv", actionMap);

    EquipUnitsRandomEquipments(allUnits);

    // Create stable pointers before any list operations
    std::vector<Unit*> allUnitPtrs;
    for (Unit& unit : allUnits) {
        allUnitPtrs.push_back(&unit);
    }

    // Split into two teams using pointers
    std::vector<Unit*> redPtrs(allUnitPtrs.begin(), allUnitPtrs.begin() + 5);
    std::vector<Unit*> bluePtrs(allUnitPtrs.begin() + 5, allUnitPtrs.end());

    // Create Battlefield with unit vectors (it will create teams internally)
    Battlefield battlefield(redPtrs, bluePtrs);
    
    // Create BattleManager with just the Battlefield
    BattleManager battleManager(battlefield);
    battleManager.StartBattle();
    
    std::cin.get();
    return 0;
}