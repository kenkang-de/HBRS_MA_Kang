#ifndef BATTLEFIELD_HPP
#define BATTLEFIELD_HPP

#include "Team.h"
#include <vector>

class Unit;

struct Battlefield
{
private: 
    Team redTeam;
    Team blueTeam;

public:
    // Constructor that creates teams from unit vectors
Battlefield(std::array<Unit*,5> redTeamPtrs, std::array<Unit*,5> blueTeamPtrs);
    
    Team* GetRedTeam() { return &redTeam; }
    Team* GetBlueTeam() { return &blueTeam; }
    
    Team* GetWinnerTeam();
    Team* GetLoserTeam();
};

#endif // BATTLEFIELD_HPP
