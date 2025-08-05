#include "Team.h"
#include "Unit.h"
#include <iostream>
#include <string>

Team::Team(TeamColor teamColor, std::vector<Unit*>& units) {
    SetTeamColor(teamColor);
    GenerateTeam(units);  // Now this will work with vector<Unit*>&
}

   const std::array<Unit*, 5>& Team::GetUnits() const {
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
            std::string actionTypeStr;
            switch(newUnit.GetWeapon().GetAction().GetActionType()) {
                case ActionType::RANGE: actionTypeStr = "RANGE"; break;
                case ActionType::MAGIC: actionTypeStr = "MAGIC"; break;
                case ActionType::MELEE: 
                default: actionTypeStr = "MELEE"; break;
            }
            std::cout << "[DEBUG] " << newUnit.GetName() << " equipped with action: " << newUnit.GetWeapon().GetAction().GetID() 
                      << " (Type: " << actionTypeStr << ")" 
                      << " Speed: " << newUnit.GetTotalStat().GetSpeed() 
                      << " Threat: " << newUnit.GetTotalStat().GetThreat() << std::endl;
            
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

void Team::GenerateTeam(std::vector<Unit*>& units)
{
    // Add up to 5 units to the team
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
            if (units[i]->IsAlive()) {
                aliveCount++;
            }
        }
    }
    
    return aliveCount == 0; // Team lost if no units are alive
}