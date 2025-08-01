#include "Battlefield.h"
#include "Unit.h"

Battlefield::Battlefield(std::vector<Unit*>& teamRed, std::vector<Unit*>& teamBlue)
    : redTeam(Red, teamRed), blueTeam(Blue, teamBlue) {
    // Teams handle everything (naming, colors, etc.) in their constructors
}

Team* Battlefield::ResultCheck() {
    if (redTeam.HasTeamLost()) {
        return &blueTeam;
    } else if (blueTeam.HasTeamLost()) {
        return &redTeam;
    }
    return nullptr;
}