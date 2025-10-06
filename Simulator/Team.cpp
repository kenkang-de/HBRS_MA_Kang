#include <iostream>
#include <string>

#include "Team.h"
#include "Unit.h"
#include "../Log/LogSystem.h"


Team::Team(TeamColor teamColor, std::array<Unit*,5> units) {
    SetTeamColor(teamColor);
    GenerateTeam(units);  // Now this will work with vector<Unit*>&
}

   const std::array<Unit*, 5>& Team::GetUnits() const {
    return units;
    }

    std::array<Unit*, 5>& Team::GetUnits() {
    return units;
    }

TeamColor Team::GetTeamColor() {
    return teamColor;
}

void Team::SetTeamColor(TeamColor color) {
    this->teamColor = color;
}

void Team::AddUnit(Unit& newUnit) {
    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i] == nullptr) {
            units[i] = &newUnit;
            SetUnitTeamColor(newUnit);
            
            // Assign name based on team color and index
            std::string prefix = (teamColor == Red) ? "R" : "B";
            newUnit.Name = prefix + std::to_string(i);
            
            // Debug output to show what each unit got
            LogSystem::LogStream("[DEBUG] " , newUnit.GetName() 
                      , " W:" , (newUnit.GetWeapon() ? newUnit.GetWeapon()->GetID() : "None") 
                      , "(" , (newUnit.GetWeapon() ? newUnit.GetWeapon()->GetAction().GetID() : "None") , ")"
                      , " A:" , (newUnit.GetArmor() ? newUnit.GetArmor()->GetID() : "None")
                      , " ATK:" , newUnit.GetTotalStat().GetAttack()
                      , " DEF:" , newUnit.GetTotalStat().GetDefense()
                      , " SPD:" , newUnit.GetTotalStat().GetSpeed() 
                      , " THR:" , newUnit.GetTotalStat().GetThreat()
                      , " HP:" , newUnit.GetTotalStat().GetHP());
            
            return;
        }
    }
}


void Team::SetUnitTeamColor(Unit& unit)
{
  unit.team = teamColor;
}

bool Team::HasPlace() {
    for (const auto& unitSlot : units) {
        if (unitSlot == nullptr) {
            return true;
        }
    }
    return false;
}

void Team::GenerateTeam(std::array<Unit*,5> units)
{
    for (size_t i = 0; i < units.size() && i < 5; ++i)
    {
        if (units[i] != nullptr) {
            this->AddUnit(*units[i]);
        }
    }
}

bool Team::HasTeamLost() {
    int aliveCount = 0;
    
    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i] != nullptr) {
            // A unit can contribute to the team if it's alive
            if (units[i]->IsAlive()) {
                aliveCount++;
            }
        }
    }
    
    return aliveCount == 0; // Team lost if no units are alive
}