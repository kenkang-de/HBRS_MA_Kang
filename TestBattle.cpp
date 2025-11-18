#include <array>
#include <iostream>

#include "Element/BattleActionLoader.h"
#include "Element/ElementList.h"
#include "Element/EquipmentLoader.h"
#include "Element/UnitGenerator.h"
#include "Paths.h"
#include "Simulator/BattleManager.h"
#include "Simulator/Battlefield.h"
#include "Simulator/Simulator.h"
#include "Simulator/Unit.h"

int main() {
    // Load actions and equipment
    std::unordered_map<std::string, BattleAction> actionMap =
        LoadActionsFromYAML("Simulator/" + Paths::BATTLE_ACTIONS_YAML);

    EquipmentLoader loader;
    ElementList elementList = loader.InstantiateElements(actionMap, "Weapon_Test.csv", "Armor_Test.csv");

    // Generate 10 units (5 red, 5 blue)
    std::array<Unit, 10> battleUnits = GenerateUnits();

    // Setup red team (units 0-4)
    for (int i = 0; i < 5; i++) {
        battleUnits[i].SetWeapon(&elementList.weapons[i]); // Choose specific weapon
        battleUnits[i].SetArmor(&elementList.armors[i]);   // Choose specific armor
        battleUnits[i].GetTotalStat() =
            battleUnits[i].GetDefaultStat() + elementList.weapons[i].GetStat() + elementList.armors[i].GetStat();
        battleUnits[i].InitializeCurrentHP();
    }

    // Setup blue team (units 5-9)
    for (int i = 5; i < 10; i++) {
        battleUnits[i].SetWeapon(&elementList.weapons[i - 5]); // Choose different weapon
        battleUnits[i].SetArmor(&elementList.armors[i - 5]);   // Choose different armor
        battleUnits[i].GetTotalStat() = battleUnits[i].GetDefaultStat() + elementList.weapons[i - 5].GetStat() +
                                        elementList.armors[i - 5].GetStat();
        battleUnits[i].InitializeCurrentHP();
    }

    // Create team pointers
    std::array<Unit *, 5> redTeamPtrs;
    std::array<Unit *, 5> blueTeamPtrs;

    for (int i = 0; i < 5; i++) {
        redTeamPtrs[i] = &battleUnits[i];
        blueTeamPtrs[i] = &battleUnits[i + 5];
    }

    // Create battlefield and start battle with logging
    Battlefield battlefield(redTeamPtrs, blueTeamPtrs);
    BattleManager battleManager(battlefield);

    std::cout << "Starting test battle..." << std::endl;
    battleManager.StartBattle(true, "TestBattle"); // true = enable logging

    std::cout << "Battle finished! Check Log/V1/TestBattle.txt for details" << std::endl;

    return 0;
}