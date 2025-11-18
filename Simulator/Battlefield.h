#ifndef BATTLEFIELD_HPP
#define BATTLEFIELD_HPP

#include "Team.h"
#include <vector>

class Unit;

enum BATTLERESULT { REDWIN, BLUEWIN, DRAW, ONGOING };

struct Battlefield {
  private:
    Team redTeam;
    Team blueTeam;

    void SetAlliesandEnemies();

  public:
    // Constructor that creates teams from unit vectors
    Battlefield(std::array<Unit *, 5> redTeamPtrs, std::array<Unit *, 5> blueTeamPtrs);

    Team *GetRedTeam() {
        return &redTeam;
    }
    Team *GetBlueTeam() {
        return &blueTeam;
    }

    BATTLERESULT GetBattleResult();
};

#endif
