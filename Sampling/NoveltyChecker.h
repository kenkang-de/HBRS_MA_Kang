#ifndef NOVELTY_CHECK_H
#define NOVELTY_CHECK_H

#include <algorithm>
#include <array>
#include <vector>

#include "../Constants.h"

class NoveltyPenalty {
  public:
    static const int overlapOutBatch = 1;
    static const int overlapInBatch = 3;
    static const int overlapInTeam = 5;
};

class NoveltyChecker {

  private:
    // Batch, Team, Unit
    std::vector<std::vector<std::array<int, 5>>> batchTeamRegistry;

    int totalBatch;
    int totalTeam;

  public:
    NoveltyChecker(int _totalBatch, int _totalTeam) : totalBatch(_totalBatch), totalTeam(_totalTeam) {
        batchTeamRegistry.resize(_totalBatch, std::vector<std::array<int, 5>>(_totalTeam));
    }

    int CheckOverlap(int batchIndex, int teamIndex, std::array<int, 5> candidateCompositeIDs);

    int ConvertToCompositeID(int totalComponent, int weaponIndex, int armorIndex);
    std::array<int, 5> ConvertToTeamCompositeIDs(int totalComponent,
                                                 std::array<std::array<int, 2>, 5> componentIndexes);

    std::array<int, 2> ConvertToComponentIndexes(int totalComponent, int compositeID);
    std::array<std::array<int, 2>, 5> ConvertToTeamComponentIndexes(int totalComponent,
                                                                    std::array<int, 5> compositeTeamIDs);

    void AddtoRegistry(int totalComponent, int batchIndex, int teamIndex, std::array<int, 5> compositeTeamIDs);

    std::array<std::array<int, 2>, 5>
    SelectNovelCandidate(int totalComponent, std::array<std::array<std::array<int, 2>, 5>, NOVELTYK> componentIndexes,
                         int candidateBatchIndex, int candidateTeamIndex);
};

#endif