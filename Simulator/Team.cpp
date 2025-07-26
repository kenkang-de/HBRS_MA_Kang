#include "Team.h"
#include "Unit.h"
#include <iostream>

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
            newUnit.Name =(prefix + std::to_string(i));

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

void Team::GenerateTeam(std::list<Unit>& units)
{
    if (units.size() != 5) {
        std::cerr << "A team must consist of exactly 5 units.\n";
        return;
    }

    for (Unit& unit : units)
    {
        this->AddUnit(unit);
    }
}
