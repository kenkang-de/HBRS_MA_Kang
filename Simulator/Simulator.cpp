#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "../Paths.h"
#include "Unit.h"
#include "UnitGenerator.h"
#include "TurnManager.h"
#include "EquipmentManager.h"
#include "SharedEquipmentManager.h"
#include "TestSubjectPersistence.h"
#include "Team.h"
#include "Battlefield.h"
#include "BattleActionLoader.h"
#include "BattleManager.h"
#include "TestSubject.h" 

// Function to equip units from configuration CSV
bool EquipUnitsFromConfig(std::list<Unit>& allUnits, const std::string& configFile, int battleIndex) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    // Skip to the requested battle configuration
    for (int i = 0; i < battleIndex && std::getline(file, line); i++) {
        // Skip lines until we reach the desired battle
    }
    
    if (!std::getline(file, line)) {
        return false;
    }
    
    // Parse the battle configuration line
    std::stringstream ss(line);
    std::string battleId;
    std::getline(ss, battleId, ','); // Skip battle ID
    
    auto unitIt = allUnits.begin();
    
    // Equip red team (units 0-4) from the configuration
    for (int i = 0; i < 5 && unitIt != allUnits.end(); i++, unitIt++) {
        std::string weaponId, armorId;
        if (std::getline(ss, weaponId, ',') && std::getline(ss, armorId, ',')) {
            // Find and equip the weapon
            for (const auto& weapon : WeaponList) {
                if (weapon.GetID() == weaponId) {
                    unitIt->SetWeapon(&weapon);
                    break;
                }
            }
            
            // Find and equip the armor
            for (const auto& armor : ArmorList) {
                if (armor.GetID() == armorId) {
                    unitIt->SetArmor(&armor);
                    break;
                }
            }
        }
    }
    
    // For blue team (units 5-9), load from configuration (columns 11-20)
    for (int i = 5; i < 10 && unitIt != allUnits.end(); i++, unitIt++) {
        std::string weaponId, armorId;
        if (std::getline(ss, weaponId, ',') && std::getline(ss, armorId, ',')) {
            // Find and equip the weapon
            for (const auto& weapon : WeaponList) {
                if (weapon.GetID() == weaponId) {
                    unitIt->SetWeapon(&weapon);
                    break;
                }
            }
            
            // Find and equip the armor
            for (const auto& armor : ArmorList) {
                if (armor.GetID() == armorId) {
                    unitIt->SetArmor(&armor);
                    break;
                }
            }
        }
    }
    
    file.close();
    return true;
}


int main(int argc, char* argv[])
{
    int numSimulations = 1; // Default to 1 simulation
    
    // Check if number of simulations was provided as argument
    if (argc > 1) {
        numSimulations = std::atoi(argv[1]);
        if (numSimulations < 1) numSimulations = 1;
    }
    
    std::cout << "Running " << numSimulations << " simulation(s)..." << std::endl;
    
    // Choose a random simulation to output to file (1-indexed)
    int selectedSim = 1 + (rand() % numSimulations);
    std::cout << "Will output simulation #" << selectedSim << " to file" << std::endl;
    
    // Initialize SharedEquipmentManager to track statistics in actual Weapon/Armor objects
    SharedEquipmentManager& equipmentManager = SharedEquipmentManager::getInstance();
    equipmentManager.loadEquipment();
    
    // Load existing TestSubject statistics from previous simulations
    TestSubjectPersistence::loadTestSubjectData();
    
    // Variables to capture selected simulation output
    std::ostringstream selectedOutput;
    std::ostringstream nullOutput; // For suppressing non-selected simulation output
    bool outputToFile = false;
    
    // Run multiple simulations
    for (int sim = 0; sim < numSimulations; sim++) {
        // Determine if this is the selected simulation to output
        outputToFile = (sim + 1 == selectedSim);
        
        // Redirect cout output - to selected stream for chosen sim, to null stream for others
        std::streambuf* coutBuf = std::cout.rdbuf(); // Save original cout buffer
        if (outputToFile) {
            std::cout.rdbuf(selectedOutput.rdbuf()); // Redirect cout to string stream for selected sim
        } else {
            std::cout.rdbuf(nullOutput.rdbuf()); // Redirect cout to null stream for non-selected sims
        }
        
        // Show progress for non-selected simulations
        if (!outputToFile && numSimulations > 1) {
            // Temporarily restore cout to show progress
            std::cout.rdbuf(coutBuf);
            std::cout << "Running simulation " << (sim + 1) << "/" << numSimulations << "..." << std::endl;
            std::cout.rdbuf(nullOutput.rdbuf()); // Redirect back to null
        }
        
        // Reseed random generator for different results each run
        ReseedRandomGenerator();
        
        std::list<Unit> allUnits = GenerateUnits(10);

        auto actionMap = LoadActionsFromYAML("BattleActions.yaml");

        // Try to load from configuration, fallback to random if not available
        std::string configFile = "battle_configs.csv";
        bool useConfig = EquipUnitsFromConfig(allUnits, configFile, sim);
        
        if (!useConfig) {
            std::cout << "[Simulator] Config loading failed, using random equipment" << std::endl;
            EquipUnitsRandomEquipments(allUnits);
        }

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
        
        // Record equipment usage for each unit using the actual Weapon/Armor TestSubject data
        for (Unit* unit : allUnitPtrs) {
            if (!unit) continue;
            
            bool unitWon = (unit->team == Red && redWon) || (unit->team == Blue && blueWon);
            
            // Update weapon stats directly in the Weapon object
            if (unit->GetWeapon()) {
                const std::string weaponId = unit->GetWeapon()->GetID();
                Weapon* weapon = equipmentManager.getMutableWeaponById(weaponId);
                if (weapon) {
                    weapon->incrementUsage();
                    if (unitWon) weapon->recordWin();
                    else if (isDraw) weapon->recordDraw();
                    else weapon->recordLoss();
                }
            }
            
            // Update armor stats directly in the Armor object
            if (unit->GetArmor()) {
                const std::string armorId = unit->GetArmor()->GetID();
                Armor* armor = equipmentManager.getMutableArmorById(armorId);
                if (armor) {
                    armor->incrementUsage();
                    if (unitWon) armor->recordWin();
                    else if (isDraw) armor->recordDraw();
                    else armor->recordLoss();
                }
            }
        }
        
        // Restore cout buffer after each simulation
        std::cout.rdbuf(coutBuf);
    }
    
    // Write the selected simulation output to file
    std::string logFileName = "Log/V1/V1test" + std::to_string(selectedSim) + ".txt";
    std::ofstream logFile(logFileName);
    logFile << selectedOutput.str();
    logFile.close();
    
    std::cout << "Saved selected simulation output to: " << logFileName << std::endl;
    
    // Output results in CSV format using actual Weapon/Armor TestSubject data
    std::cout << "EQUIPMENT_ANALYSIS_START" << std::endl;
    std::cout << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // Get all weapons and sort by win rate
    std::vector<Weapon*> sortedWeapons;
    for (Weapon& weapon : equipmentManager.getAllWeapons()) {
        if (weapon.UsageCount > 0) {
            sortedWeapons.push_back(&weapon);
        }
    }
    std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
              [](const Weapon* a, const Weapon* b) { return a->WinRate > b->WinRate; });
    
    for (const Weapon* weapon : sortedWeapons) {
        std::cout << "Weapon," << weapon->GetID() << "," 
                  << std::fixed << std::setprecision(0) << (weapon->WinRate * 100) << ","
                  << weapon->TotalWin << "," << weapon->TotalLost << "," 
                  << weapon->TotalDraw << "," << weapon->UsageCount << std::endl;
    }
    
    // Get all armor and sort by win rate
    std::vector<Armor*> sortedArmor;
    for (Armor& armor : equipmentManager.getAllArmor()) {
        if (armor.UsageCount > 0) {
            sortedArmor.push_back(&armor);
        }
    }
    std::sort(sortedArmor.begin(), sortedArmor.end(), 
              [](const Armor* a, const Armor* b) { return a->WinRate > b->WinRate; });
    
    for (const Armor* armor : sortedArmor) {
        std::cout << "Armor," << armor->GetID() << "," 
                  << std::fixed << std::setprecision(0) << (armor->WinRate * 100) << ","
                  << armor->TotalWin << "," << armor->TotalLost << "," 
                  << armor->TotalDraw << "," << armor->UsageCount << std::endl;
    }
    
    std::cout << "EQUIPMENT_ANALYSIS_END" << std::endl;
    
    // Save accumulated TestSubject statistics for next simulation
    TestSubjectPersistence::saveTestSubjectData();
    
    return 0;
}