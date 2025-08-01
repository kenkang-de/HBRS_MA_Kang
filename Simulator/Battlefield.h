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
    Battlefield(std::vector<Unit*>& teamRed, std::vector<Unit*>& teamBlue);
    
    Team* GetRedTeam() { return &redTeam; }
    Team* GetBlueTeam() { return &blueTeam; }
    
    Team* ResultCheck();
};

#endif // BATTLEFIELD_HPP
