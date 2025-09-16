#ifndef TEAM_H
#define TEAM_H

#include <array>
#include <vector> 

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
    Team(TeamColor teamColor, std::array<Unit*,5> units); 
    const std::array<Unit*, 5>& GetUnits() const;

    TeamColor GetTeamColor();
    void SetTeamColor(TeamColor teamColor);
    void GenerateTeam(std::array<Unit*,5> units);
    void AddUnit(Unit& newUnit);
    bool HasTeamLost();
    bool HasPlace();
};

#endif
