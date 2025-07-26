
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
    LoadWeaponListFromCSV("SpeedTest.csv",actionMap);

    EquipUnitsRandomEquipments(allUnits);

    // Split into two teams
    std::list<Unit> redUnits;
    std::list<Unit> blueUnits;


auto it = allUnits.begin();
std::advance(it, 5);

redUnits.splice(redUnits.begin(), allUnits, allUnits.begin(), it);
blueUnits.splice(blueUnits.begin(), allUnits, allUnits.begin(), allUnits.end());

    Team redTeam(TeamColor::Red ,redUnits);
    Team blueTeam(TeamColor::Blue ,blueUnits);

    Battlefield battlefield(redTeam, blueTeam);

    std::vector<Unit*> redPtrs, bluePtrs;
for (Unit* u : redTeam.GetUnits()) redPtrs.push_back(u);
for (Unit* u : blueTeam.GetUnits()) bluePtrs.push_back(u);

    BattleManager battleManager(redPtrs, bluePtrs);
    battleManager.StartBattle();
std::cin.get(); // Waits for Enter key
    return 0;
}