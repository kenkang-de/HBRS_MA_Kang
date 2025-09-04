#include "SamplingController.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

SamplingController::SamplingController(int totalTeams, int teamsPerBatch, int targetSims) 
    : totalTeams(totalTeams), teamsPerBatch(teamsPerBatch), targetSimulations(targetSims), rng(std::random_device{}()) {
}

bool SamplingController::loadTeamConfigurations(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "[SamplingController] Could not open config file: " << configFile << std::endl;
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    int teamIndex = 0;
    while (std::getline(file, line) && teamIndex < totalTeams) {
        std::stringstream ss(line);
        std::string battleId;
        std::getline(ss, battleId, ',');
        
        // Extract Red Team (first 5 weapon-armor pairs)
        TeamConfiguration redTeam;
        redTeam.teamId = "Red_" + std::to_string(teamIndex / 2 + 1);
        
        for (int i = 0; i < 5; i++) {
            std::string weaponId, armorId;
            if (std::getline(ss, weaponId, ',') && std::getline(ss, armorId, ',')) {
                redTeam.units.push_back({weaponId, armorId});
                redTeam.weaponUsage[weaponId]++;
                redTeam.armorUsage[armorId]++;
                
                // Track global usage
                globalWeaponUsage[weaponId]++;
                globalArmorUsage[armorId]++;
                combinationUsage[weaponId + "+" + armorId]++;
            }
        }
        
        allTeams.push_back(redTeam);
        teamIndex++;
        
        if (teamIndex >= totalTeams) break;
        
        // Extract Blue Team (next 5 weapon-armor pairs)
        TeamConfiguration blueTeam;
        blueTeam.teamId = "Blue_" + std::to_string(teamIndex / 2 + 1);
        
        for (int i = 0; i < 5; i++) {
            std::string weaponId, armorId;
            if (std::getline(ss, weaponId, ',') && std::getline(ss, armorId, ',')) {
                blueTeam.units.push_back({weaponId, armorId});
                blueTeam.weaponUsage[weaponId]++;
                blueTeam.armorUsage[armorId]++;
                
                // Track global usage
                globalWeaponUsage[weaponId]++;
                globalArmorUsage[armorId]++;
                combinationUsage[weaponId + "+" + armorId]++;
            }
        }
        
        allTeams.push_back(blueTeam);
        teamIndex++;
    }
    
    std::cout << "[SamplingController] Loaded " << allTeams.size() << " team configurations" << std::endl;
    return true;
}

bool SamplingController::distributeToBatches() {
    if (allTeams.size() < static_cast<size_t>(totalTeams)) {
        std::cerr << "[SamplingController] Not enough teams loaded: " << allTeams.size() << "/" << totalTeams << std::endl;
        return false;
    }
    
    // Shuffle teams for random distribution
    std::shuffle(allTeams.begin(), allTeams.end(), rng);
    
    // Distribute teams into batches
    int numBatches = totalTeams / teamsPerBatch;
    batches.reserve(numBatches);
    
    for (int batchId = 0; batchId < numBatches; batchId++) {
        BatchConfiguration batch;
        batch.batchId = batchId + 1;
        batch.batchFile = "batch_" + std::to_string(batch.batchId) + "_config.csv";
        
        // Assign teams to this batch
        for (int teamIdx = 0; teamIdx < teamsPerBatch; teamIdx++) {
            int globalTeamIdx = batchId * teamsPerBatch + teamIdx;
            if (globalTeamIdx < static_cast<int>(allTeams.size())) {
                batch.teams.push_back(allTeams[globalTeamIdx]);
            }
        }
        
        batches.push_back(batch);
    }
    
    std::cout << "[SamplingController] Distributed " << totalTeams << " teams into " << batches.size() << " batches" << std::endl;
    return true;
}

bool SamplingController::exportBatches(const std::string& outputDir) {
    for (const auto& batch : batches) {
        std::string filepath = outputDir + "/" + batch.batchFile;
        std::ofstream file(filepath);
        
        if (!file.is_open()) {
            std::cerr << "[SamplingController] Could not create batch file: " << filepath << std::endl;
            return false;
        }
        
        // Write header
        file << "BattleId,RedTeam_W1,RedTeam_A1,RedTeam_W2,RedTeam_A2,RedTeam_W3,RedTeam_A3,RedTeam_W4,RedTeam_A4,RedTeam_W5,RedTeam_A5,";
        file << "BlueTeam_W1,BlueTeam_A1,BlueTeam_W2,BlueTeam_A2,BlueTeam_W3,BlueTeam_A3,BlueTeam_W4,BlueTeam_A4,BlueTeam_W5,BlueTeam_A5\\n";
        
        // Write team configurations for round-robin battles
        for (size_t i = 0; i < batch.teams.size(); i++) {
            for (size_t j = i + 1; j < batch.teams.size(); j++) {
                const auto& teamA = batch.teams[i];
                const auto& teamB = batch.teams[j];
                
                std::string battleId = "Batch" + std::to_string(batch.batchId) + "_" + teamA.teamId + "vs" + teamB.teamId;
                file << battleId;
                
                // Write Team A (Red) equipment
                for (int unit = 0; unit < 5 && unit < static_cast<int>(teamA.units.size()); unit++) {
                    file << "," << teamA.units[unit].first << "," << teamA.units[unit].second;
                }
                
                // Write Team B (Blue) equipment  
                for (int unit = 0; unit < 5 && unit < static_cast<int>(teamB.units.size()); unit++) {
                    file << "," << teamB.units[unit].first << "," << teamB.units[unit].second;
                }
                
                file << "\\n";
            }
        }
        
        file.close();
    }
    
    std::cout << "[SamplingController] Exported " << batches.size() << " batch configuration files" << std::endl;
    return true;
}

void SamplingController::printSamplingStats() const {
    std::cout << "\\n=== Sampling Statistics ===" << std::endl;
    std::cout << "Total Teams: " << allTeams.size() << std::endl;
    std::cout << "Batches: " << batches.size() << std::endl;
    std::cout << "Teams per Batch: " << teamsPerBatch << std::endl;
    std::cout << "Matches per Batch: " << (teamsPerBatch * (teamsPerBatch - 1)) / 2 << std::endl;
    std::cout << "Total Matches: " << batches.size() * ((teamsPerBatch * (teamsPerBatch - 1)) / 2) << std::endl;
    
    std::cout << "\\nEquipment Usage Distribution:" << std::endl;
    std::cout << "Unique Weapons: " << globalWeaponUsage.size() << std::endl;
    std::cout << "Unique Armor: " << globalArmorUsage.size() << std::endl;
    std::cout << "Unique Combinations: " << combinationUsage.size() << std::endl;
}
