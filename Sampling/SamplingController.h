#ifndef SAMPLING_CONTROLLER_H
#define SAMPLING_CONTROLLER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <random>

struct TeamConfiguration {
    std::string teamId;
    std::vector<std::pair<std::string, std::string>> units; // weapon+armor pairs
    std::map<std::string, int> weaponUsage;
    std::map<std::string, int> armorUsage;
};

struct BatchConfiguration {
    int batchId;
    std::vector<TeamConfiguration> teams; // 10 teams per batch
    std::string batchFile;
};

class SamplingController {
private:
    // Configuration parameters
    int totalTeams;
    int teamsPerBatch;
    int targetSimulations;
    
    std::vector<TeamConfiguration> allTeams;
    std::vector<BatchConfiguration> batches;
    std::mt19937 rng;
    
    // Usage tracking for balancing
    std::map<std::string, int> globalWeaponUsage;
    std::map<std::string, int> globalArmorUsage;
    std::map<std::string, int> combinationUsage;
    
public:
    SamplingController(int totalTeams = 200, int teamsPerBatch = 10, int targetSims = 2000);
    
    // Core sampling methods
    bool loadTeamConfigurations(const std::string& configFile);
    bool distributeToBatches();
    bool exportBatches(const std::string& outputDir);
    
    // Sampling strategies
    bool balanceEquipmentUsage();
    bool ensureNovelty();
    bool validateDistribution();
    
    // Statistics and reporting
    void printSamplingStats() const;
    void exportSamplingReport(const std::string& filename) const;
    
    // Getters
    const std::vector<BatchConfiguration>& getBatches() const { return batches; }
    int getNumBatches() const { return batches.size(); }
};

#endif
