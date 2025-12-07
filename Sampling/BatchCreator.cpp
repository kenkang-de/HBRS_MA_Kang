#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../Simulator/Armor.h"
#include "../Simulator/TestSubject.h"
#include "../Simulator/Weapon.h"
#include "Batch.h"
#include "BatchCreator.h"

// How many times does it randomly selects equipment when sampling.
// When it hits the limit, then the equipment with the lowest usage and  order
const int RerollLimit = 3;

int BatchCreator::totalComponent = 0;

BatchConfig BatchCreator::CreateBatchConfig(int numBatches, int teamsPerBatch, std::vector<Weapon> *weaponList,
                                            std::vector<Armor> *armorList) {

    BatchConfig config;
    config.numBatches = numBatches;
    config.teamsPerBatch = teamsPerBatch;

    for (const Weapon &weapon : *weaponList) {
        EquipmentConfig weaponConfig;
        weaponConfig.id = weapon.GetID();
        config.weaponConfigs.push_back(weaponConfig);
    }
    for (const Armor &armor : *armorList) {
        EquipmentConfig armorConfig;
        armorConfig.id = armor.GetID();
        config.armorConfigs.push_back(armorConfig);
    }
    return config;
}

int BatchCreator::GetRandomEquipmentIndex() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, totalComponent - 1);
    return dis(gen);
}

std::vector<Batch> BatchCreator::CreateBatches(BatchConfig batchConfig) {

    std::vector<Batch> batches(batchConfig.numBatches);

    int unitCount = batchConfig.numBatches * batchConfig.teamsPerBatch * 5;
    int weaponCount = batchConfig.weaponConfigs.size();
    int armorCount = batchConfig.armorConfigs.size();

    if (weaponCount != armorCount) {
        std::cerr << "ERROR: Weapon and Armor count mismatch!" << std::endl;
        return {};
    }

    int totalComponent = weaponCount;
    SetTotalComponent(totalComponent);

    int baseUsage = unitCount / totalComponent;
    int remainder = unitCount % totalComponent;
    int minSampleUsage = baseUsage;
    int maxSampleUsage = baseUsage + (remainder > 0 ? 1 : 0);

    // Initialize Batch ID and team vectors.
    for (int batchIndex = 0; batchIndex < batchConfig.numBatches; batchIndex++) {

        batches[batchIndex].batchId = "Batch_" + std::to_string(batchIndex);
        batches[batchIndex].teams = std::vector<SimulationTeamSetting>(batchConfig.teamsPerBatch);
    }

    // Distribute Weapon and Armor to teams
    int totalTeamCount = batchConfig.numBatches * batchConfig.teamsPerBatch;

    NoveltyChecker noveltyChecker(batchConfig.numBatches, totalTeamCount);
    std::array<std::array<std::array<int, 2>, 5>, NOVELTYK> candidates;

    for (int totalTeamIndex = 0; totalTeamIndex < totalTeamCount; totalTeamIndex++) {

        std::array<std::array<int, 2>, 5> componentIndexes; // [weaponIndex, armorIndex] for each unit
        int batchIndex = totalTeamIndex / batchConfig.teamsPerBatch;
        int teamIndex = totalTeamIndex % batchConfig.teamsPerBatch;

        for (int candidateNumber = 0; candidateNumber < NOVELTYK; candidateNumber++) {
            // Assign equipment for 5 units in this team
            for (int unitIndex = 0; unitIndex < 5; unitIndex++) {
                int ranCount = 0;

                // Weapon
                int ranEquipmentIndex = GetRandomEquipmentIndex();
                while (ranCount < RerollLimit &&
                       batchConfig.weaponConfigs[ranEquipmentIndex].SampleUsageCount >= maxSampleUsage) {
                    ranEquipmentIndex = GetRandomEquipmentIndex();
                    ranCount++;
                }
                int finalWeaponIndex =
                    ranCount >= RerollLimit ? batchConfig.GetLowestUsageWeaponIndex() : ranEquipmentIndex;

                ranCount = 0;

                // Armor
                ranEquipmentIndex = GetRandomEquipmentIndex();
                while (ranCount < RerollLimit &&
                       batchConfig.armorConfigs[ranEquipmentIndex].SampleUsageCount >= maxSampleUsage) {
                    ranEquipmentIndex = GetRandomEquipmentIndex();
                    ranCount++;
                }
                int finalArmorIndex =
                    ranCount >= RerollLimit ? batchConfig.GetLowestUsageArmorIndex() : ranEquipmentIndex;

                candidates[candidateNumber][unitIndex][0] = finalWeaponIndex;
                candidates[candidateNumber][unitIndex][1] = finalArmorIndex;
            }
        }
        std::array<std::array<int, 2>, 5> novelCandidate =
            noveltyChecker.SelectNovelCandidate(totalComponent, candidates, batchIndex, teamIndex);

        // Team-wise operations after collecting all component indexes
        for (int unitIndex = 0; unitIndex < 5; unitIndex++) {
            int finalWeaponIndex = novelCandidate[unitIndex][0];
            int finalArmorIndex = novelCandidate[unitIndex][1];

            SimulationUnitSetting newUnitSetting;
            newUnitSetting.weaponID = batchConfig.weaponConfigs[finalWeaponIndex].id;
            newUnitSetting.armorID = batchConfig.armorConfigs[finalArmorIndex].id;

            batches[batchIndex].teams[teamIndex].simulationUnitSettings[unitIndex] = newUnitSetting;

            batchConfig.weaponConfigs[finalWeaponIndex].SampleUsageCount++;
            batchConfig.armorConfigs[finalArmorIndex].SampleUsageCount++;
        }
    }

    return batches;
}

int BatchConfig::GetLowestUsageArmorIndex() {
    int lowestUsageIndex = 0;

    for (int i = 0; i < armorConfigs.size(); i++) {
        if (armorConfigs[i].SampleUsageCount < armorConfigs[lowestUsageIndex].SampleUsageCount) {
            lowestUsageIndex = i;
        } else if (armorConfigs[i].SampleUsageCount == armorConfigs[lowestUsageIndex].SampleUsageCount) {
        }
    }
    return lowestUsageIndex;
}

int BatchConfig::GetLowestUsageWeaponIndex() {
    int lowestUsageIndex = 0;

    for (int i = 0; i < weaponConfigs.size(); i++) {
        if (weaponConfigs[i].SampleUsageCount < weaponConfigs[lowestUsageIndex].SampleUsageCount) {
            lowestUsageIndex = i;
        } else if (weaponConfigs[i].SampleUsageCount == weaponConfigs[lowestUsageIndex].SampleUsageCount) {
        }
    }
    return lowestUsageIndex;
}
