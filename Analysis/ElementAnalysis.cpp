#include "ElementAnalysis.h"
#include "../Paths.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

// Static instance for singleton pattern
ElementAnalysis* ElementAnalysis::instance = nullptr;

ElementAnalysis::ElementAnalysis() {
    // std::cout << "[ElementAnalysis] Initialized in-memory element tracking." << std::endl;
    loadPersistentData();
}

ElementAnalysis::~ElementAnalysis() {
    savePersistentData();
}

void ElementAnalysis::loadPersistentData() {
    std::ifstream file(persistentFile);
    if (!file.is_open()) {
        // std::cout << "[ElementAnalysis] No existing data file found, starting fresh." << std::endl;
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string type, id;
        int totalWin, totalLost, totalDraw, usageCount;
        float winRate;
        
        if (std::getline(ss, type, ',') &&
            std::getline(ss, id, ',') &&
            ss >> winRate && ss.ignore() &&
            ss >> totalWin && ss.ignore() &&
            ss >> totalLost && ss.ignore() &&
            ss >> totalDraw && ss.ignore() &&
            ss >> usageCount) {
            
            TestSubject stats(id);
            stats.TotalWin = totalWin;
            stats.TotalLost = totalLost;
            stats.TotalDraw = totalDraw;
            stats.UsageCount = usageCount;
            stats.WinRate = winRate;
            
            if (type == "Weapon") {
                weaponStats[id] = stats;
            } else if (type == "Armor") {
                armorStats[id] = stats;
            }
        }
    }
    
    std::cout << "[ElementAnalysis] Loaded " << weaponStats.size() << " weapons and " 
              << armorStats.size() << " armor pieces from persistent data." << std::endl;
}

void ElementAnalysis::savePersistentData() {
    std::ofstream file(persistentFile);
    if (!file.is_open()) {
        std::cerr << "[ElementAnalysis] Error: Cannot save to " << persistentFile << std::endl;
        return;
    }
    
    // Write header
    file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount\n";
    
    // Write weapon stats
    for (const auto& pair : weaponStats) {
        const TestSubject& stats = pair.second;
        file << "Weapon," << stats.GetID() << "," 
             << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
             << stats.TotalWin << "," << stats.TotalLost << "," 
             << stats.TotalDraw << "," << stats.UsageCount << "\n";
    }
    
    // Write armor stats
    for (const auto& pair : armorStats) {
        const TestSubject& stats = pair.second;
        file << "Armor," << stats.GetID() << "," 
             << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
             << stats.TotalWin << "," << stats.TotalLost << "," 
             << stats.TotalDraw << "," << stats.UsageCount << "\n";
    }
}

ElementAnalysis* ElementAnalysis::getInstance() {
    if (!ElementAnalysis::instance) {
        ElementAnalysis::instance = new ElementAnalysis();
    }
    return ElementAnalysis::instance;
}

void ElementAnalysis::recordWeaponUsage(const std::string& weaponId, bool won, bool draw) {
    TestSubject& stats = weaponStats[weaponId]; // Creates entry if doesn't exist
    if (stats.GetID().empty()) {
        stats = TestSubject(weaponId); // Initialize with ID if new
    }
    
    stats.UsageCount++;
    if (won) {
        stats.TotalWin++;
    } else if (draw) {
        stats.TotalDraw++;
    } else {
        stats.TotalLost++;
    }
    
    // Recalculate win rate
    stats.WinRate = calculateWinRate(stats.TotalWin, stats.TotalDraw, stats.UsageCount);
}

void ElementAnalysis::recordArmorUsage(const std::string& armorId, bool won, bool draw) {
    TestSubject& stats = armorStats[armorId]; // Creates entry if doesn't exist
    if (stats.GetID().empty()) {
        stats = TestSubject(armorId); // Initialize with ID if new
    }
    
    stats.UsageCount++;
    if (won) {
        stats.TotalWin++;
    } else if (draw) {
        stats.TotalDraw++;
    } else {
        stats.TotalLost++;
    }
    
    // Recalculate win rate
    stats.WinRate = calculateWinRate(stats.TotalWin, stats.TotalDraw, stats.UsageCount);
}

void ElementAnalysis::recordEquipmentUsage(const std::string& weaponId, const std::string& armorId, bool won, bool draw) {
    recordWeaponUsage(weaponId, won, draw);
    recordArmorUsage(armorId, won, draw);
}

float ElementAnalysis::calculateWinRate(int totalWin, int totalDraw, int usageCount) {
    if (usageCount == 0) return 0.0f;
    
    float rawWinRate = (static_cast<float>(totalWin) + (0.5f * static_cast<float>(totalDraw))) / static_cast<float>(usageCount);
    
    // Round to 2 decimal places
    return std::round(rawWinRate * 100.0f) / 100.0f;
}

void ElementAnalysis::generateAnalysisReport(const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "[ElementAnalysis] Error: Cannot create analysis file " << outputPath << std::endl;
        return;
    }
    
    // Write header
    file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount\n";
    
    // Sort and write weapon stats by win rate (descending)
    std::vector<std::pair<std::string, TestSubject*>> sortedWeapons;
    for (auto& pair : weaponStats) {
        sortedWeapons.push_back({pair.first, &pair.second});
    }
    std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
              [](const auto& a, const auto& b) { return a.second->WinRate > b.second->WinRate; });
    
    for (const auto& pair : sortedWeapons) {
        const TestSubject& stats = *pair.second;
        file << "Weapon," << stats.GetID() << "," 
             << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
             << stats.TotalWin << "," << stats.TotalLost << "," 
             << stats.TotalDraw << "," << stats.UsageCount << "\n";
    }
    
    // Sort and write armor stats by win rate (descending)
    std::vector<std::pair<std::string, TestSubject*>> sortedArmor;
    for (auto& pair : armorStats) {
        sortedArmor.push_back({pair.first, &pair.second});
    }
    std::sort(sortedArmor.begin(), sortedArmor.end(), 
              [](const auto& a, const auto& b) { return a.second->WinRate > b.second->WinRate; });
    
    for (const auto& pair : sortedArmor) {
        const TestSubject& stats = *pair.second;
        file << "Armor," << stats.GetID() << "," 
             << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
             << stats.TotalWin << "," << stats.TotalLost << "," 
             << stats.TotalDraw << "," << stats.UsageCount << "\n";
    }
    
    // std::cout << "[ElementAnalysis] Equipment analysis saved to " << outputPath << std::endl;
    // std::cout << "[ElementAnalysis] Analyzed " << weaponStats.size() << " weapons and " 
    //           << armorStats.size() << " armor pieces." << std::endl;
}

void ElementAnalysis::printCurrentStats() {
    std::cout << "\n=== Current Element Statistics ===" << std::endl;
    std::cout << "Weapons tracked: " << weaponStats.size() << std::endl;
    std::cout << "Armor pieces tracked: " << armorStats.size() << std::endl;
    
    for (const auto& pair : weaponStats) {
        const TestSubject& stats = pair.second;
        std::cout << "Weapon " << stats.GetID() << ": " << stats.UsageCount << " uses, " 
                  << std::fixed << std::setprecision(2) << stats.WinRate << " win rate" << std::endl;
    }
}
