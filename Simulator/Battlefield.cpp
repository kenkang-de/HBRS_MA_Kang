#include "Battlefield.h"
#include "Unit.h"

Battlefield::Battlefield(std::array<Unit *, 5> redTeamPtrs, std::array<Unit *, 5> blueTeamPtrs)
    : redTeam(Red, redTeamPtrs), blueTeam(Blue, blueTeamPtrs) {
    SetAlliesandEnemies();
}

Team *Battlefield::GetWinnerTeam() {
    if (redTeam.HasTeamLost()) {
        return &blueTeam;
    } else if (blueTeam.HasTeamLost()) {
        return &redTeam;
    }
    return nullptr;
}

Team *Battlefield::GetLoserTeam() {
    if (redTeam.HasTeamLost()) {
        return &redTeam;
    } else if (blueTeam.HasTeamLost()) {
        return &blueTeam;
    }
    return nullptr;
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