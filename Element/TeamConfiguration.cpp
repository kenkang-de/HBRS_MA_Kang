#include "TeamConfiguration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

TeamConfiguration::TeamConfiguration() : rng(std::random_device{}()) {}

bool TeamConfiguration::initialize() {
    std::cout << "[TeamConfiguration] Initializing team configuration system..." << std::endl;
    
    // Load all equipment using centralized loader
    if (!equipmentLoader.loadAllEquipment()) {
        std::cerr << "[TeamConfiguration] Failed to load equipment data" << std::endl;
        return false;
    }
    
    equipmentLoader.printSummary();
    return true;
}

std::string TeamConfiguration::getComboKey(const std::string& weaponId, const std::string& armorId) const {
    return weaponId + "+" + armorId;
}

std::string TeamConfiguration::getTeamKey(const TeamConfig& team) const {
    std::string key;
    for (const auto& unit : team.units) {
        if (!key.empty()) key += "|";
        key += getComboKey(unit.weapon->GetID(), unit.armor->GetID());
    }
    return key;
}

bool TeamConfiguration::isValidEquipmentPair(const std::string& weaponId, const std::string& armorId) const {
    const std::string combo = getComboKey(weaponId, armorId);
    auto it = comboUsage.find(combo);
    return it == comboUsage.end() || it->second < 2; // Max 2 uses per combo
}

bool TeamConfiguration::isValidTeam(const TeamConfig& team) const {
    const std::string teamKey = getTeamKey(team);
    return usedTeams.find(teamKey) == usedTeams.end();
}

UnitConfig TeamConfiguration::generateValidUnit() {
    const auto& weapons = equipmentLoader.getWeapons();
    const auto& armor = equipmentLoader.getArmor();
    
    std::uniform_int_distribution<> weaponDist(0, weapons.size() - 1);
    std::uniform_int_distribution<> armorDist(0, armor.size() - 1);
    
    int attempts = 0;
    const int maxAttempts = 1000;
    
    while (attempts < maxAttempts) {
        int weaponIdx = weaponDist(rng);
        int armorIdx = armorDist(rng);
        
        const std::string& weaponId = weapons[weaponIdx].GetID();
        const std::string& armorId = armor[armorIdx].GetID();
        
        if (isValidEquipmentPair(weaponId, armorId)) {
            return UnitConfig(
                const_cast<SimpleWeapon*>(&weapons[weaponIdx]),
                const_cast<SimpleArmor*>(&armor[armorIdx])
            );
        }
        attempts++;
    }
    
    // Fallback: return first valid combination
    return UnitConfig(
        const_cast<SimpleWeapon*>(&weapons[0]),
        const_cast<SimpleArmor*>(&armor[0])
    );
}

TeamConfig TeamConfiguration::generateValidTeam(const std::string& teamId) {
    TeamConfig team(teamId);
    
    int attempts = 0;
    const int maxAttempts = 1000;
    
    while (attempts < maxAttempts) {
        team.units.clear();
        
        // Generate 5 units for the team
        for (int i = 0; i < 5; ++i) {
            team.units.push_back(generateValidUnit());
        }
        
        if (isValidTeam(team)) {
            // Update usage tracking
            const std::string teamKey = getTeamKey(team);
            usedTeams.insert(teamKey);
            
            for (const auto& unit : team.units) {
                weaponUsage[unit.weapon->GetID()]++;
                armorUsage[unit.armor->GetID()]++;
                comboUsage[getComboKey(unit.weapon->GetID(), unit.armor->GetID())]++;
                
                // Update usage count in equipment
                unit.weapon->incrementUsage();
                unit.armor->incrementUsage();
            }
            
            return team;
        }
        attempts++;
    }
    
    std::cerr << "[TeamConfiguration] Warning: Could not generate unique team after " 
              << maxAttempts << " attempts" << std::endl;
    return team;
}

void TeamConfiguration::generateBattleConfigs(int count) {
    std::cout << "[TeamConfiguration] Generating " << count << " battle configurations..." << std::endl;
    
    battleConfigs.clear();
    battleConfigs.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        BattleConfig config("Battle_" + std::to_string(i + 1));
        
        config.redTeam = generateValidTeam("Red_" + std::to_string(i + 1));
        config.blueTeam = generateValidTeam("Blue_" + std::to_string(i + 1));
        
        battleConfigs.push_back(std::move(config));
        
        if ((i + 1) % 10 == 0) {
            std::cout << "[TeamConfiguration] Generated " << (i + 1) << "/" << count 
                      << " configurations..." << std::endl;
        }
    }
    
    std::cout << "[TeamConfiguration] Successfully generated " << battleConfigs.size() 
              << " battle configurations" << std::endl;
}

bool TeamConfiguration::exportBattleConfigs(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[TeamConfiguration] Error: Could not create file: " << filename << std::endl;
        return false;
    }
    
    // Write header
    file << "BattleID,RedWeapon1,RedArmor1,RedWeapon2,RedArmor2,RedWeapon3,RedArmor3,RedWeapon4,RedArmor4,RedWeapon5,RedArmor5,"
         << "BlueWeapon1,BlueArmor1,BlueWeapon2,BlueArmor2,BlueWeapon3,BlueArmor3,BlueWeapon4,BlueArmor4,BlueWeapon5,BlueArmor5" << std::endl;
    
    for (const auto& config : battleConfigs) {
        file << config.battleId;
        
        // Red team equipment
        for (const auto& unit : config.redTeam.units) {
            file << "," << unit.weapon->GetID() << "," << unit.armor->GetID();
        }
        
        // Blue team equipment
        for (const auto& unit : config.blueTeam.units) {
            file << "," << unit.weapon->GetID() << "," << unit.armor->GetID();
        }
        
        file << std::endl;
    }
    
    std::cout << "[TeamConfiguration] Exported " << battleConfigs.size() 
              << " configurations to: " << filename << std::endl;
    return true;
}

void TeamConfiguration::printStats() const {
    std::cout << "\n=== Team Configuration Statistics ===" << std::endl;
    std::cout << "Battle Configurations: " << battleConfigs.size() << std::endl;
    std::cout << "Unique Weapon Usage: " << weaponUsage.size() << std::endl;
    std::cout << "Unique Armor Usage: " << armorUsage.size() << std::endl;
    std::cout << "Unique Combinations: " << comboUsage.size() << std::endl;
    std::cout << "Unique Teams: " << usedTeams.size() << std::endl;
    
    equipmentLoader.printSummary();
}

void TeamConfiguration::printFinalAnalysis() const {
    std::cout << "EQUIPMENT_ANALYSIS_START" << std::endl;
    std::cout << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // Sort weapons by win rate
    std::vector<const SimpleWeapon*> sortedWeapons;
    for (const auto& weapon : equipmentLoader.getWeapons()) {
        if (weapon.UsageCount > 0) {
            sortedWeapons.push_back(&weapon);
        }
    }
    std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
              [](const auto* a, const auto* b) { return a->WinRate > b->WinRate; });
    
    for (const auto* weapon : sortedWeapons) {
        std::cout << "Weapon," << weapon->GetID() << "," 
                  << std::fixed << std::setprecision(0) << (weapon->WinRate * 100) << ","
                  << weapon->TotalWin << "," << weapon->TotalLost << "," 
                  << weapon->TotalDraw << "," << weapon->UsageCount << std::endl;
    }
    
    // Sort armor by win rate
    std::vector<const SimpleArmor*> sortedArmor;
    for (const auto& armorItem : equipmentLoader.getArmor()) {
        if (armorItem.UsageCount > 0) {
            sortedArmor.push_back(&armorItem);
        }
    }
    std::sort(sortedArmor.begin(), sortedArmor.end(), 
              [](const auto* a, const auto* b) { return a->WinRate > b->WinRate; });
    
    for (const auto* armorItem : sortedArmor) {
        std::cout << "Armor," << armorItem->GetID() << "," 
                  << std::fixed << std::setprecision(0) << (armorItem->WinRate * 100) << ","
                  << armorItem->TotalWin << "," << armorItem->TotalLost << "," 
                  << armorItem->TotalDraw << "," << armorItem->UsageCount << std::endl;
    }
    
    std::cout << "EQUIPMENT_ANALYSIS_END" << std::endl;
}

void TeamConfiguration::exportFinalAnalysis(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[TeamConfiguration] Error: Could not create analysis file: " << filename << std::endl;
        return;
    }
    
    file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // Export weapon statistics
    for (const auto& weapon : equipmentLoader.getWeapons()) {
        if (weapon.UsageCount > 0) {
            file << "Weapon," << weapon.GetID() << "," 
                 << std::fixed << std::setprecision(0) << (weapon.WinRate * 100) << ","
                 << weapon.TotalWin << "," << weapon.TotalLost << "," 
                 << weapon.TotalDraw << "," << weapon.UsageCount << std::endl;
        }
    }
    
    // Export armor statistics
    for (const auto& armorItem : equipmentLoader.getArmor()) {
        if (armorItem.UsageCount > 0) {
            file << "Armor," << armorItem.GetID() << "," 
                 << std::fixed << std::setprecision(0) << (armorItem.WinRate * 100) << ","
                 << armorItem.TotalWin << "," << armorItem.TotalLost << "," 
                 << armorItem.TotalDraw << "," << armorItem.UsageCount << std::endl;
        }
    }
    
    std::cout << "[TeamConfiguration] Final analysis exported to: " << filename << std::endl;
}

void TeamConfiguration::resetAllResults() {
    equipmentLoader.resetAllResults();
    weaponUsage.clear();
    armorUsage.clear();
    comboUsage.clear();
    usedTeams.clear();
    std::cout << "[TeamConfiguration] All battle results and constraints reset" << std::endl;
}
