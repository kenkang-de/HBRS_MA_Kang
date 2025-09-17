#include "Battlefield.h"
#include "Unit.h"

Battlefield::Battlefield(std::array<Unit*,5> redTeamPtrs, std::array<Unit*,5> blueTeamPtrs)
    : redTeam(Red, redTeamPtrs), blueTeam(Blue, blueTeamPtrs) {
}

Team* Battlefield::GetWinnerTeam() {
    if (redTeam.HasTeamLost()) {
        return &blueTeam;
    } else if (blueTeam.HasTeamLost()) {
        return &redTeam;
    }
    return nullptr;
}

Team* Battlefield::GetLoserTeam() {
    if (redTeam.HasTeamLost()) {
        return &redTeam;
    } else if (blueTeam.HasTeamLost()) {
        return &blueTeam;
    }
    return nullptr;
}