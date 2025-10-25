#include <iostream>
#include <vector>

#include "BattleManager.h"
#include "Constants.h"
#include "Simulator.h"
#include "SynergyRule.h"

#include "../Log/LogSystem.h"

void Simulator::Init_Battlefield() {
    std::array<Unit *, 5> redTeamPtrs;
    std::array<Unit *, 5> blueTeamPtrs;

    for (int i = 0; i < UNITS_PER_TEAM; i++) {
        redTeamPtrs[i] = &(*battleUnits)[i];
        blueTeamPtrs[i] = &(*battleUnits)[i + UNITS_PER_TEAM];
    }

    battlefield = std::make_unique<Battlefield>(redTeamPtrs, blueTeamPtrs);
}

void Simulator::SimulateBatches(std::vector<Batch> *batches) {
    // Iterate through batches, teams
    for (Batch &batch : *batches) {
        // Round-Robin Tournament: Each team plays against every other team
        for (int teamA = 0; teamA < batch.teams.size(); teamA++) {
            for (int teamB = teamA + 1; teamB < batch.teams.size(); teamB++) {
                // First battle of the batch will be logged, and saved to Log directory
                SimulateBattle(&batch.teams[teamA], &batch.teams[teamB], teamA == 0 && teamB == 1, batch.batchId);
            }
        }
    }
    LogSystem::StopLogging();
}

void Simulator::SimulateBatches(std::vector<Batch> *batches, Chromosome *chromosome) {
    chromosome->ApplyStatToComponents(elementList);

    // Iterate through batches, teams
    for (Batch &batch : *batches) {
        // Round-Robin Tournament: Each team plays against every other team
        for (int teamA = 0; teamA < batch.teams.size(); teamA++) {
            for (int teamB = teamA + 1; teamB < batch.teams.size(); teamB++) {
                // First battle of the batch will be logged, and saved to Log directory
                SimulateBattle(&batch.teams[teamA], &batch.teams[teamB], false, batch.batchId);
            }
        }
    }
    //
}

void Simulator::EquipTeam(int startIndex, SimulationTeamSetting *teamSetting) {
    for (int i = 0; i < UNITS_PER_TEAM; i++) {
        Unit *unit = &(*battleUnits)[startIndex + i];
        SimulationUnitSetting &unitSetting = teamSetting->simulationUnitSettings[i];

        unit->ResetUnit();

        Armor *armor = elementList->FindArmorByID(unitSetting.armorID);
        if (armor) {
            unit->SetArmor(armor);
        }

        Weapon *weapon = elementList->FindWeaponByID(unitSetting.weaponID);
        if (weapon) {
            unit->SetWeapon(weapon);
        }

        SynergyRule::ApplyUnitSynergyMatch(unit);
    }
    //    SynergyRule::PrintTotalUnitSynergyApplied();
}

void Simulator::SimulateBattle(SimulationTeamSetting *redTeamSetting, SimulationTeamSetting *blueTeamSetting, bool log,
                               std::string batchID) {
    // Clear any static/global state before battle
    SynergyRule::ResetAppliedCounter(); // Reset synergy counter each battle
    GlobalAction::ClearAfterAction();   // Clear any lingering after actions

    // Equip both teams
    EquipTeam(0, redTeamSetting);
    EquipTeam(UNITS_PER_TEAM, blueTeamSetting);

    BattleManager battleManager(*battlefield);
    battleManager.StartBattle(log, batchID);

    // Additional cleanup after battle
    GlobalAction::ClearAfterAction(); // Make sure after actions are cleared
}
