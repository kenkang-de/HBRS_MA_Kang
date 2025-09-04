#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>
#include <algorithm>

// Simple Equipment classes without YAML dependency
struct SimpleEquipment {
    std::string id;
    float WinRate = 0.0f;
    int TotalWin = 0;
    int TotalLost = 0;
    int TotalDraw = 0;
    int UsageCount = 0;
    
    void recordWin() {
        TotalWin++;
        updateWinRate();
    }
    
    void recordLoss() {
        TotalLost++;
        updateWinRate();
    }
    
    void recordDraw() {
        TotalDraw++;
        updateWinRate();
    }
    
    void incrementUsage() {
        UsageCount++;
    }
    
private:
    void updateWinRate() {
        int totalBattles = TotalWin + TotalLost + TotalDraw;
        if (totalBattles > 0) {
            WinRate = static_cast<float>(TotalWin) / totalBattles * 100.0f;
        } else {
            WinRate = 0.0f;
        }
    }
};

class SimpleDirectAnalyzer {
private:
    std::vector<SimpleEquipment> weapons;
    std::vector<SimpleEquipment> armors;
    
public:
    // Load equipment statistics from the battle_equipment_data.txt files
    bool loadStatisticsFromSimulationLogs() {
        std::cout << "[SimpleDirectAnalyzer] Loading statistics from simulation logs..." << std::endl;
        
        // Initialize equipment maps with all possible equipment IDs (BSW1-BSW50, BSA1-BSA50)
        std::map<std::string, SimpleEquipment> weaponMap;
        std::map<std::string, SimpleEquipment> armorMap;
        
        // Pre-populate all equipment IDs to ensure 50 weapons and 50 armor are always analyzed
        for (int i = 1; i <= 50; i++) {
            std::string weaponId = "BSW" + std::to_string(i);
            std::string armorId = "BSA" + std::to_string(i);
            weaponMap[weaponId].id = weaponId;
            armorMap[armorId].id = armorId;
        }
        
        // Process all Batch*_V1test*.txt files in the Log/V1 directory
        std::string logDir = "../Log/V1/";
        int filesProcessed = 0;
        
        // Since we don't have filesystem in older C++, let's try to process known batch files
        for (int i = 1; i <= 20; i++) {
            std::string filename = logDir + "Batch" + std::to_string(i) + "_V1test2.txt";
            std::ifstream file(filename);
            
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    // Look for equipment debug lines
                    // Format: [DEBUG] Unit R0: W=BSW32 A=BSA23 Won=1 Draw=0
                    if (line.find("[DEBUG] Unit ") != std::string::npos && 
                        line.find("W=") != std::string::npos && 
                        line.find("A=") != std::string::npos) {
                        
                        std::string weaponId, armorId;
                        int won = 0, draw = 0;
                        
                        // Extract weapon ID
                        size_t weaponPos = line.find("W=");
                        if (weaponPos != std::string::npos) {
                            weaponPos += 2;
                            size_t weaponEnd = line.find(" ", weaponPos);
                            weaponId = line.substr(weaponPos, weaponEnd - weaponPos);
                        }
                        
                        // Extract armor ID  
                        size_t armorPos = line.find("A=");
                        if (armorPos != std::string::npos) {
                            armorPos += 2;
                            size_t armorEnd = line.find(" ", armorPos);
                            armorId = line.substr(armorPos, armorEnd - armorPos);
                        }
                        
                        // Extract won status
                        size_t wonPos = line.find("Won=");
                        if (wonPos != std::string::npos) {
                            won = (line[wonPos + 4] == '1') ? 1 : 0;
                        }
                        
                        // Extract draw status  
                        size_t drawPos = line.find("Draw=");
                        if (drawPos != std::string::npos) {
                            draw = (line[drawPos + 5] == '1') ? 1 : 0;
                        }
                        
                        // Update weapon statistics
                        if (!weaponId.empty()) {
                            if (weaponMap.find(weaponId) == weaponMap.end()) {
                                weaponMap[weaponId].id = weaponId;
                            }
                            weaponMap[weaponId].incrementUsage();
                            if (won) weaponMap[weaponId].recordWin();
                            else if (draw) weaponMap[weaponId].recordDraw();
                            else weaponMap[weaponId].recordLoss();
                        }
                        
                        // Update armor statistics
                        if (!armorId.empty()) {
                            if (armorMap.find(armorId) == armorMap.end()) {
                                armorMap[armorId].id = armorId;
                            }
                            armorMap[armorId].incrementUsage();
                            if (won) armorMap[armorId].recordWin();
                            else if (draw) armorMap[armorId].recordDraw();
                            else armorMap[armorId].recordLoss();
                        }
                    }
                }
                file.close();
                filesProcessed++;
            }
        }
        
        // Convert maps to vectors
        for (const auto& pair : weaponMap) {
            weapons.push_back(pair.second);
        }
        for (const auto& pair : armorMap) {
            armors.push_back(pair.second);
        }
        
        std::cout << "[SimpleDirectAnalyzer] Processed " << filesProcessed << " log files" << std::endl;
        std::cout << "[SimpleDirectAnalyzer] Found " << weapons.size() << " weapons, " << armors.size() << " armor pieces" << std::endl;
        
        return filesProcessed > 0;
    }
    
    void generateAnalysisReport(const std::string& outputPath = "../Log/V1/DirectEquipmentAnalysis.csv") {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            std::cerr << "[SimpleDirectAnalyzer] Error: Could not create output file: " << outputPath << std::endl;
            return;
        }
        
        std::cout << "[SimpleDirectAnalyzer] Generating direct equipment analysis report..." << std::endl;
        
        // Write CSV header
        file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
        
        // Sort weapons by win rate (descending)
        std::sort(weapons.begin(), weapons.end(), [](const SimpleEquipment& a, const SimpleEquipment& b) {
            return a.WinRate > b.WinRate;
        });
        
        // Write weapon stats
        for (const SimpleEquipment& weapon : weapons) {
            file << "Weapon," << weapon.id << "," 
                 << std::fixed << std::setprecision(0) << weapon.WinRate << ","
                 << weapon.TotalWin << "," << weapon.TotalLost << "," 
                 << weapon.TotalDraw << "," << weapon.UsageCount << std::endl;
        }
        
        // Sort armor by win rate (descending)
        std::sort(armors.begin(), armors.end(), [](const SimpleEquipment& a, const SimpleEquipment& b) {
            return a.WinRate > b.WinRate;
        });
        
        // Write armor stats
        for (const SimpleEquipment& armor : armors) {
            file << "Armor," << armor.id << "," 
                 << std::fixed << std::setprecision(0) << armor.WinRate << ","
                 << armor.TotalWin << "," << armor.TotalLost << "," 
                 << armor.TotalDraw << "," << armor.UsageCount << std::endl;
        }
        
        file.close();
        
        printSummary();
        
        std::cout << "[SimpleDirectAnalyzer] Direct equipment analysis saved to: " << outputPath << std::endl;
    }
    
    void printSummary() {
        int totalWeaponUsage = 0, totalArmorUsage = 0;
        
        for (const auto& weapon : weapons) {
            totalWeaponUsage += weapon.UsageCount;
        }
        for (const auto& armor : armors) {
            totalArmorUsage += armor.UsageCount;
        }
        
        std::cout << "\n=== Simple Direct Equipment Analysis Summary ===" << std::endl;
        std::cout << "Weapons Analyzed: " << weapons.size() << std::endl;
        std::cout << "Armor Analyzed: " << armors.size() << std::endl;
        std::cout << "Total Weapon Usage: " << totalWeaponUsage << std::endl;
        std::cout << "Total Armor Usage: " << totalArmorUsage << std::endl;
        
        // Find best performers
        if (!weapons.empty()) {
            const auto& bestWeapon = *std::max_element(weapons.begin(), weapons.end(), 
                [](const SimpleEquipment& a, const SimpleEquipment& b) { return a.WinRate < b.WinRate; });
            std::cout << "Best Weapon: " << bestWeapon.id 
                      << " (" << std::fixed << std::setprecision(1) << bestWeapon.WinRate << "% win rate)" << std::endl;
        }
        
        if (!armors.empty()) {
            const auto& bestArmor = *std::max_element(armors.begin(), armors.end(), 
                [](const SimpleEquipment& a, const SimpleEquipment& b) { return a.WinRate < b.WinRate; });
            std::cout << "Best Armor: " << bestArmor.id 
                      << " (" << std::fixed << std::setprecision(1) << bestArmor.WinRate << "% win rate)" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Simple Direct Equipment Analysis Tool ===" << std::endl;
    
    SimpleDirectAnalyzer analyzer;
    
    if (!analyzer.loadStatisticsFromSimulationLogs()) {
        std::cerr << "Failed to load statistics from simulation logs!" << std::endl;
        return 1;
    }
    
    analyzer.generateAnalysisReport();
    
    std::cout << "\n=== Simple Direct Analysis Complete ===" << std::endl;
    return 0;
}
