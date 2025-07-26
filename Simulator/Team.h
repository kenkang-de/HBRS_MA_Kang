#ifndef TEAM_H
#define TEAM_H

#include <array>
#include <list>

class Unit;  

enum TeamColor {
    Red, Blue
};

class Team {
private:
    std::array<Unit*, 5> units{};
    TeamColor teamColor;
    void SetUnitTeamColor(Unit& unit);

public:
    Team();
    Team(TeamColor teamColor, std::list<Unit>& units)
    { 
        SetTeamColor(teamColor);
        GenerateTeam(units); } 

      const std::array<Unit*, 5>& GetUnits() const;
 

    TeamColor GetTeamColor();
    void SetTeamColor(TeamColor teamColor);
    void GenerateTeam(std::list<Unit>& units);
    void AddUnit(Unit& newUnit);
    bool HasPlace();
};

#endif
