#ifndef ELEMENT_ANALYSIS_H
#define ELEMENT_ANALYSIS_H

#include "../Simulator/TestSubject.h"
#include <map>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>

class ElementAnalysis {
private:
    std::map<std::string, TestSubject> weaponStats;
    std::map<std::string, TestSubject> armorStats;
    
    std::string persistentFile = "../Log/V1/ElementAnalysisV1_Data.csv";
    
    // Static instance for singleton
    static ElementAnalysis* instance;

public:
    ElementAnalysis();
    ~ElementAnalysis();
    
    // Load/save persistent data
    void loadPersistentData();
    void savePersistentData();
    
    // Record usage and outcome for weapons/armor (in-memory only)
    void recordWeaponUsage(const std::string& weaponId, bool won, bool draw);
    void recordArmorUsage(const std::string& armorId, bool won, bool draw);
    
    // Simplified interface - record equipment usage by IDs
    void recordEquipmentUsage(const std::string& weaponId, const std::string& armorId, bool won, bool draw);
    
    // Calculate win rate with proper rounding
    float calculateWinRate(int totalWin, int totalDraw, int usageCount);
    
    // Generate final analysis report to CSV (only when requested)
    void generateAnalysisReport(const std::string& outputPath);
    
    // Get current statistics (for debugging)
    void printCurrentStats();
    
    // Static instance for global access
    static ElementAnalysis* getInstance();

};

#endif // ELEMENT_ANALYSIS_H
