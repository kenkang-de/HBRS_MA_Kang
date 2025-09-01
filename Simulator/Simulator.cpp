#include <list>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "Unit.h"
#include "UnitGenerator.h"
#include "TurnManager.h"
#include "EquipmentManager.h"
#include "Team.h"
#include "Battlefield.h"
#include "BattleActionLoader.h"
#include "BattleManager.h"
#include "TestSubject.h" 


int main(int argc, char* argv[])
{
    int numSimulations = 1; // Default to 1 simulation
    
    // Check if number of simulations was provided as argument
    if (argc > 1) {
        numSimulations = std::atoi(argv[1]);
        if (numSimulations < 1) numSimulations = 1;
    }
    
    std::cout << "Running " << numSimulations << " simulation(s)..." << std::endl;
    
    // Equipment tracking using TestSubject structure
    std::map<std::string, TestSubject> weaponStats;
    std::map<std::string, TestSubject> armorStats;
    
    // Run multiple simulations
    for (int sim = 0; sim < numSimulations; sim++) {
        // Redirect output to individual log file for this simulation
        std::string logFileName = "../Log/V1/V1test" + std::to_string(sim + 1) + ".txt";
        std::ofstream logFile(logFileName);
        std::streambuf* coutBuf = std::cout.rdbuf(); // Save original cout buffer
        std::cout.rdbuf(logFile.rdbuf()); // Redirect cout to file
        
        // Reseed random generator for different results each run
        ReseedRandomGenerator();
        
        std::list<Unit> allUnits = GenerateUnits(10);

        auto actionMap = LoadActionsFromYAML("BattleActions.yaml");

        LoadArmorListFromCSV("Data/Armor_v1.csv");
        LoadWeaponListFromCSV("Data/Weapon_v1.csv", actionMap);

        EquipUnitsRandomEquipments(allUnits);

        // Create stable pointers before any list operations
        std::vector<Unit*> allUnitPtrs;
        for (Unit& unit : allUnits) {
            allUnitPtrs.push_back(&unit);
        }

        // Split into two teams using pointers
        std::vector<Unit*> redPtrs(allUnitPtrs.begin(), allUnitPtrs.begin() + 5);
        std::vector<Unit*> bluePtrs(allUnitPtrs.begin() + 5, allUnitPtrs.end());

        // Create Battlefield with unit vectors (it will create teams internally)
        Battlefield battlefield(redPtrs, bluePtrs);
        
        // Create BattleManager with just the Battlefield
        BattleManager battleManager(battlefield);
        battleManager.StartBattle();
        
        // After battle ends, record equipment usage using TestSubject structure
        Team* winner = battlefield.ResultCheck();
        bool redWon = (winner && winner->GetTeamColor() == Red);
        bool blueWon = (winner && winner->GetTeamColor() == Blue);
        bool isDraw = !redWon && !blueWon;
        
        // Record equipment usage for each unit
        for (Unit* unit : allUnitPtrs) {
            if (!unit) continue;
            
            bool unitWon = (unit->team == Red && redWon) || (unit->team == Blue && blueWon);
            
            // Get equipment IDs
            const std::string& weaponId = unit->GetWeapon().GetID();
            const std::string& armorId = unit->GetArmor().GetID();
            
            // Update weapon stats using TestSubject structure
            if (weaponStats.find(weaponId) == weaponStats.end()) {
                weaponStats[weaponId] = TestSubject(weaponId);
            }
            TestSubject& weaponStat = weaponStats[weaponId];
            weaponStat.UsageCount++;
            if (unitWon) weaponStat.TotalWin++;
            else if (isDraw) weaponStat.TotalDraw++;
            else weaponStat.TotalLost++;
            
            // Update armor stats using TestSubject structure
            if (armorStats.find(armorId) == armorStats.end()) {
                armorStats[armorId] = TestSubject(armorId);
            }
            TestSubject& armorStat = armorStats[armorId];
            armorStat.UsageCount++;
            if (unitWon) armorStat.TotalWin++;
            else if (isDraw) armorStat.TotalDraw++;
            else armorStat.TotalLost++;
        }
        
        // Restore original cout buffer
        std::cout.rdbuf(coutBuf);
        logFile.close();
    }
    
    // Calculate win rates for all equipment
    for (auto& pair : weaponStats) {
        TestSubject& stats = pair.second;
        if (stats.UsageCount > 0) {
            stats.WinRate = (static_cast<float>(stats.TotalWin) + (0.5f * static_cast<float>(stats.TotalDraw))) / static_cast<float>(stats.UsageCount);
        }
    }
    
    for (auto& pair : armorStats) {
        TestSubject& stats = pair.second;
        if (stats.UsageCount > 0) {
            stats.WinRate = (static_cast<float>(stats.TotalWin) + (0.5f * static_cast<float>(stats.TotalDraw))) / static_cast<float>(stats.UsageCount);
        }
    }
    
    // Output results in CSV format for RunTests to capture
    std::cout << "EQUIPMENT_ANALYSIS_START" << std::endl;
    std::cout << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // Output weapon stats sorted by win rate
    std::vector<std::pair<std::string, TestSubject*>> sortedWeapons;
    for (auto& pair : weaponStats) {
        sortedWeapons.push_back({pair.first, &pair.second});
    }
    std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
              [](const auto& a, const auto& b) { return a.second->WinRate > b.second->WinRate; });
    
    for (const auto& pair : sortedWeapons) {
        const TestSubject& stats = *pair.second;
        std::cout << "Weapon," << stats.GetID() << "," 
                  << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
                  << stats.TotalWin << "," << stats.TotalLost << "," 
                  << stats.TotalDraw << "," << stats.UsageCount << std::endl;
    }
    
    // Output armor stats sorted by win rate
    std::vector<std::pair<std::string, TestSubject*>> sortedArmor;
    for (auto& pair : armorStats) {
        sortedArmor.push_back({pair.first, &pair.second});
    }
    std::sort(sortedArmor.begin(), sortedArmor.end(), 
              [](const auto& a, const auto& b) { return a.second->WinRate > b.second->WinRate; });
    
    for (const auto& pair : sortedArmor) {
        const TestSubject& stats = *pair.second;
        std::cout << "Armor," << stats.GetID() << "," 
                  << std::fixed << std::setprecision(0) << (stats.WinRate * 100) << ","
                  << stats.TotalWin << "," << stats.TotalLost << "," 
                  << stats.TotalDraw << "," << stats.UsageCount << std::endl;
    }
    
    std::cout << "EQUIPMENT_ANALYSIS_END" << std::endl;
    
    return 0;
}