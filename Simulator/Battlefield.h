#ifndef BATTLEFIELD_HPP
#define BATTLEFIELD_HPP

#include "Team.h"

struct Battlefield
{
    private: 
    

    public:

    Battlefield(Team& redTeam, Team& blueTeam)
    : RedTeam(&redTeam), BlueTeam(&blueTeam)
    {}


    Team* RedTeam;
    Team* BlueTeam;
};

#endif // BATTLEFIELD_HPP
