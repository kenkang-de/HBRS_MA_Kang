#include "NoveltyChecker.h"

int NoveltyChecker::ConvertToCompositeID(int totalComponent, int weaponIndex, int armorIndex) {
    return weaponIndex * totalComponent + armorIndex;
}

std::array<int, 5> NoveltyChecker::ConvertToTeamCompositeIDs(int totalComponent,
                                                             std::array<std::array<int, 2>, 5> componentIndexes) {
    std::array<int, 5> compositeIDs;
    for (int i = 0; i < compositeIDs.size(); i++) {
        // Weapon first!
        int compositeID = ConvertToCompositeID(totalComponent, componentIndexes[i][0], componentIndexes[i][1]);
        compositeIDs[i] = compositeID;
    }
    std::sort(compositeIDs.begin(), compositeIDs.end());
    return compositeIDs;
}

std::array<int, 2> NoveltyChecker::ConvertToComponentIndexes(int totalComponent, int compositeID) {
    int weaponIndex = compositeID / totalComponent;
    int armorIndex = compositeID % totalComponent;
    return {weaponIndex, armorIndex};
}

std::array<std::array<int, 2>, 5> NoveltyChecker::ConvertToTeamComponentIndexes(int totalComponent,
                                                                                std::array<int, 5> compositeTeamIDs) {
    std::array<std::array<int, 2>, 5> componentIndexes;
    for (int i = 0; i < compositeTeamIDs.size(); i++) {
        componentIndexes[i] = ConvertToComponentIndexes(totalComponent, compositeTeamIDs[i]);
    }
    return componentIndexes;
}

int NoveltyChecker::CheckOverlap(int candidateBatchIndex, int candidateTeamIndex,
                                 std::array<int, 5> candidateCompositeIDs) {

    int penalty = 0;
    // Overlap outer from the corresponding batch
    int overlapOutBatch = 0;
    int overlapInBatch = 0;
    int overlapInTeam = 0;

    // Check overlap in Team
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (i != j && candidateCompositeIDs[i] == candidateCompositeIDs[j])
                overlapInTeam++;
        }
    }

    for (int candidateID : candidateCompositeIDs) {
        for (size_t registryBatchIndex = 0; registryBatchIndex < batchTeamRegistry.size(); registryBatchIndex++) {
            for (size_t registryTeamIndex = 0; registryTeamIndex < batchTeamRegistry[registryBatchIndex].size();
                 registryTeamIndex++) {
                for (int registeredCompositeID : batchTeamRegistry[registryBatchIndex][registryTeamIndex]) {
                    if (candidateID == registeredCompositeID && candidateBatchIndex == registryBatchIndex) {
                        overlapInBatch++;
                    } else if (candidateID == registeredCompositeID && candidateBatchIndex != registryBatchIndex) {
                        overlapOutBatch++;
                    }
                }
            }
        }
    }

    penalty = overlapOutBatch * NoveltyPenalty::overlapOutBatch + overlapInBatch * NoveltyPenalty::overlapInBatch +
              overlapInTeam * NoveltyPenalty::overlapInTeam;

    return penalty;
}

void NoveltyChecker::AddtoRegistry(int totalComponent, int batchIndex, int teamIndex,
                                   std::array<int, 5> compositeTeamIDs) {
    batchTeamRegistry[batchIndex][teamIndex] = compositeTeamIDs;
}

std::array<std::array<int, 2>, 5>
NoveltyChecker::SelectNovelCandidate(int totalComponent,
                                     std::array<std::array<std::array<int, 2>, 5>, NOVELTYK> componentIndexes,
                                     int candidateBatchIndex, int candidateTeamIndex) {
    // Initialize with first candidate
    std::array<int, 5> lowestPenaltyTeam = ConvertToTeamCompositeIDs(totalComponent, componentIndexes[0]);
    std::array<int, 5> compositeTeamIDs;
    int lowestPenalty = CheckOverlap(candidateBatchIndex, candidateTeamIndex, lowestPenaltyTeam);

    // Skips the other candidate when first candiate novelty penalty is 0
    if (lowestPenalty != 0) {
        // Start from second candidate
        for (size_t i = 1; i < componentIndexes.size(); i++) {
            compositeTeamIDs = ConvertToTeamCompositeIDs(totalComponent, componentIndexes[i]);
            int penalty = CheckOverlap(candidateBatchIndex, candidateTeamIndex, compositeTeamIDs);

            if (penalty < lowestPenalty) {
                lowestPenalty = penalty;
                lowestPenaltyTeam = compositeTeamIDs;
            }
        }
    }

    AddtoRegistry(totalComponent, candidateBatchIndex, candidateTeamIndex, lowestPenaltyTeam);
    return ConvertToTeamComponentIndexes(totalComponent, lowestPenaltyTeam);
}
