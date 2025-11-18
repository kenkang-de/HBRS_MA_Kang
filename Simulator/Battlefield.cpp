#include "Battlefield.h"
#include "Unit.h"

Battlefield::Battlefield(std::array<Unit *, 5> redTeamPtrs, std::array<Unit *, 5> blueTeamPtrs)
    : redTeam(Red, redTeamPtrs), blueTeam(Blue, blueTeamPtrs) {
    SetAlliesandEnemies();
}

BATTLERESULT Battlefield::GetBattleResult() {
    bool redLost = redTeam.HasTeamLost();
    bool blueLost = blueTeam.HasTeamLost();

    if (redLost && blueLost) {
        return DRAW;
    }
    if (redLost) {
        return BLUEWIN;
    } else if (blueLost) {
        return REDWIN;
    }
    return ONGOING;
}

void Battlefield::SetAlliesandEnemies() {
    for (Unit *redTeamUnit : redTeam.GetUnits()) {
        for (Unit *otherRedUnit : redTeam.GetUnits()) {
            redTeamUnit->Allies.push_back(otherRedUnit);
        }

        for (Unit *blueTeamUnit : blueTeam.GetUnits()) {
            redTeamUnit->Enemies.push_back(blueTeamUnit);
        }
    }

    for (Unit *blueTeamUnit : blueTeam.GetUnits()) {
        for (Unit *otherBlueUnit : blueTeam.GetUnits()) {
            blueTeamUnit->Allies.push_back(otherBlueUnit);
        }

        for (Unit *redTeamUnit : redTeam.GetUnits()) {
            blueTeamUnit->Enemies.push_back(redTeamUnit);
        }
    }
}