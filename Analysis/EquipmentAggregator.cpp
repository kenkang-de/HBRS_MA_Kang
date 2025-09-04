#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <filesystem>

struct EquipmentStats {
    std::string id;
    int totalWin = 0;
    int totalLost = 0;
    int totalDraw = 0;
    int usageCount = 0;
    float winRate = 0.0f;
    
    void calculateWinRate() {
        if (usageCount > 0) {
            winRate = ((float)totalWin + (0.5f * (float)totalDraw)) / (float)usageCount * 100.0f;
        }
    }
};

class EquipmentAnalysisAggregator {
private:
    std::map<std::string, EquipmentStats> weaponStats;
    std::map<std::string, EquipmentStats> armorStats;
    std::string logDirectory;
    
public:
    EquipmentAnalysisAggregator(const std::string& logDir = "../Log/V1/") 
        : logDirectory(logDir) {}
    
    bool aggregateAllBatchResults() {
        std::cout << "[EquipmentAggregator] Starting equipment analysis aggregation..." << std::endl;
        
        int processedFiles = 0;
        int totalSimulations = 0;
        
        // Process all batch simulation log files
        try {
            for (const auto& entry : std::filesystem::directory_iterator(logDirectory)) {
                if (entry.path().extension() == ".txt") {
                    std::string filename = entry.path().filename().string();
                    
                    // Only process batch result files (Batch*_V1test*.txt)
                    if (filename.find("Batch") == 0 && filename.find("_V1test") != std::string::npos) {
                        if (processBattleLogFile(entry.path().string())) {
                            processedFiles++;
                            totalSimulations++;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[EquipmentAggregator] Error reading log directory: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << "[EquipmentAggregator] Processed " << processedFiles << " simulation files" << std::endl;
        
        if (processedFiles == 0) {
            std::cerr << "[EquipmentAggregator] No batch simulation files found!" << std::endl;
            return false;
        }
        
        // Calculate win rates for all equipment
        for (auto& pair : weaponStats) {
            pair.second.calculateWinRate();
        }
        
        for (auto& pair : armorStats) {
            pair.second.calculateWinRate();
        }
        
        return true;
    }
    
    bool exportAggregatedAnalysis(const std::string& outputFile = "EquipmentAnalysis_Aggregated.csv") {
        std::string fullPath = logDirectory + outputFile;
        std::ofstream file(fullPath);
        
        if (!file.is_open()) {
            std::cerr << "[EquipmentAggregator] Could not create output file: " << fullPath << std::endl;
            return false;
        }
        
        // Write CSV header
        file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
        
        // Sort weapons by win rate (descending)
        std::vector<std::pair<std::string, EquipmentStats*>> sortedWeapons;
        for (auto& pair : weaponStats) {
            sortedWeapons.push_back({pair.first, &pair.second});
        }
        std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
                  [](const auto& a, const auto& b) { return a.second->winRate > b.second->winRate; });
        
        // Write weapon stats
        for (const auto& pair : sortedWeapons) {
            const EquipmentStats& stats = *pair.second;
            file << "Weapon," << stats.id << "," 
                 << std::fixed << std::setprecision(0) << stats.winRate << ","
                 << stats.totalWin << "," << stats.totalLost << "," 
                 << stats.totalDraw << "," << stats.usageCount << std::endl;
        }
        
        // Sort armor by win rate (descending)
        std::vector<std::pair<std::string, EquipmentStats*>> sortedArmor;
        for (auto& pair : armorStats) {
            sortedArmor.push_back({pair.first, &pair.second});
        }
        std::sort(sortedArmor.begin(), sortedArmor.end(), 
                  [](const auto& a, const auto& b) { return a.second->winRate > b.second->winRate; });
        
        // Write armor stats
        for (const auto& pair : sortedArmor) {
            const EquipmentStats& stats = *pair.second;
            file << "Armor," << stats.id << "," 
                 << std::fixed << std::setprecision(0) << stats.winRate << ","
                 << stats.totalWin << "," << stats.totalLost << "," 
                 << stats.totalDraw << "," << stats.usageCount << std::endl;
        }
        
        file.close();
        
        std::cout << "[EquipmentAggregator] Aggregated analysis saved to: " << fullPath << std::endl;
        return true;
    }
    
    void printSummary() {
        std::cout << "\n=== Equipment Analysis Summary ===" << std::endl;
        std::cout << "Unique Weapons Analyzed: " << weaponStats.size() << std::endl;
        std::cout << "Unique Armor Analyzed: " << armorStats.size() << std::endl;
        
        int totalWeaponUsage = 0, totalArmorUsage = 0;
        for (const auto& pair : weaponStats) {
            totalWeaponUsage += pair.second.usageCount;
        }
        for (const auto& pair : armorStats) {
            totalArmorUsage += pair.second.usageCount;
        }
        
        std::cout << "Total Weapon Usage: " << totalWeaponUsage << std::endl;
        std::cout << "Total Armor Usage: " << totalArmorUsage << std::endl;
        
        // Find top performers
        if (!weaponStats.empty()) {
            auto bestWeapon = std::max_element(weaponStats.begin(), weaponStats.end(),
                [](const auto& a, const auto& b) { return a.second.winRate < b.second.winRate; });
            std::cout << "Best Weapon: " << bestWeapon->first 
                      << " (" << std::fixed << std::setprecision(1) << bestWeapon->second.winRate << "% win rate)" << std::endl;
        }
        
        if (!armorStats.empty()) {
            auto bestArmor = std::max_element(armorStats.begin(), armorStats.end(),
                [](const auto& a, const auto& b) { return a.second.winRate < b.second.winRate; });
            std::cout << "Best Armor: " << bestArmor->first 
                      << " (" << std::fixed << std::setprecision(1) << bestArmor->second.winRate << "% win rate)" << std::endl;
        }
    }
    
private:
    bool processBattleLogFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        
        // Look for the equipment recording section at the end
        while (std::getline(file, line)) {
            if (line.find("[DEBUG] Unit") != std::string::npos && 
                line.find("Won=") != std::string::npos && 
                line.find("Draw=") != std::string::npos) {
                
                // Parse equipment stats line
                // Format: [DEBUG] Unit R0: W=BSW32 A=BSA23 Won=0 Draw=0
                std::string weaponId, armorId;
                int won = 0, draw = 0;
                
                // Extract weapon ID
                size_t weaponPos = line.find("W=");
                if (weaponPos != std::string::npos) {
                    weaponPos += 2; // Skip "W="
                    size_t weaponEnd = line.find(" ", weaponPos);
                    if (weaponEnd != std::string::npos) {
                        weaponId = line.substr(weaponPos, weaponEnd - weaponPos);
                    }
                }
                
                // Extract armor ID
                size_t armorPos = line.find("A=");
                if (armorPos != std::string::npos) {
                    armorPos += 2; // Skip "A="
                    size_t armorEnd = line.find(" ", armorPos);
                    if (armorEnd != std::string::npos) {
                        armorId = line.substr(armorPos, armorEnd - armorPos);
                    }
                }
                
                // Extract Won value
                size_t wonPos = line.find("Won=");
                if (wonPos != std::string::npos) {
                    wonPos += 4; // Skip "Won="
                    size_t wonEnd = line.find(" ", wonPos);
                    if (wonEnd != std::string::npos) {
                        won = std::stoi(line.substr(wonPos, wonEnd - wonPos));
                    }
                }
                
                // Extract Draw value
                size_t drawPos = line.find("Draw=");
                if (drawPos != std::string::npos) {
                    drawPos += 5; // Skip "Draw="
                    draw = std::stoi(line.substr(drawPos));
                }
                
                // Update weapon stats
                if (!weaponId.empty()) {
                    EquipmentStats& stats = weaponStats[weaponId];
                    stats.id = weaponId;
                    stats.usageCount++;
                    
                    if (won == 1) {
                        stats.totalWin++;
                    } else if (draw == 1) {
                        stats.totalDraw++;
                    } else {
                        stats.totalLost++;
                    }
                }
                
                // Update armor stats
                if (!armorId.empty()) {
                    EquipmentStats& stats = armorStats[armorId];
                    stats.id = armorId;
                    stats.usageCount++;
                    
                    if (won == 1) {
                        stats.totalWin++;
                    } else if (draw == 1) {
                        stats.totalDraw++;
                    } else {
                        stats.totalLost++;
                    }
                }
            }
        }
        
        return true;
    }
};

int main() {
    std::cout << "=== Equipment Analysis Aggregator ===" << std::endl;
    
    EquipmentAnalysisAggregator aggregator("../Log/V1/");
    
    if (!aggregator.aggregateAllBatchResults()) {
        std::cerr << "Failed to aggregate batch results!" << std::endl;
        return 1;
    }
    
    aggregator.printSummary();
    
    if (!aggregator.exportAggregatedAnalysis()) {
        std::cerr << "Failed to export aggregated analysis!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Equipment Analysis Complete ===" << std::endl;
    return 0;
}
