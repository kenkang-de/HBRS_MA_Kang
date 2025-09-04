#ifndef TEAM_CONFIGURATION_H
#define TEAM_CONFIGURATION_H

#include "EquipmentLoader.h"
#include "../Paths.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <random>

// Configuration structures
struct UnitConfig {
    SimpleWeapon* weapon;
    SimpleArmor* armor;
    
    UnitConfig(SimpleWeapon* w, SimpleArmor* a) : weapon(w), armor(a) {}
};

struct TeamConfig {
    std::vector<UnitConfig> units;
    std::string teamId;
    
    TeamConfig(const std::string& id) : teamId(id) {
        units.reserve(5); // Each team has 5 units
    }
};

struct BattleConfig {
    TeamConfig redTeam;
    TeamConfig blueTeam;
    std::string battleId;
    
    BattleConfig(const std::string& id) 
        : redTeam("Red_" + id), blueTeam("Blue_" + id), battleId(id) {}
};

// Clean team configuration generator using centralized equipment
class TeamConfiguration {
private:
    EquipmentLoader equipmentLoader;
    std::vector<BattleConfig> battleConfigs;
    
    // Constraint tracking
    std::unordered_map<std::string, int> weaponUsage;
    std::unordered_map<std::string, int> armorUsage;
    std::unordered_map<std::string, int> comboUsage;
    std::unordered_set<std::string> usedTeams;
    
    // Random number generation
    std::mt19937 rng;
    
    // Helper methods
    std::string getComboKey(const std::string& weaponId, const std::string& armorId) const;
    std::string getTeamKey(const TeamConfig& team) const;
    bool isValidEquipmentPair(const std::string& weaponId, const std::string& armorId) const;
    bool isValidTeam(const TeamConfig& team) const;
    UnitConfig generateValidUnit();
    TeamConfig generateValidTeam(const std::string& teamId);
    
public:
    TeamConfiguration();
    
    // Core functionality
    bool initialize();
    void generateBattleConfigs(int count);
    
    // Export/Import
    bool exportBattleConfigs(const std::string& filename) const;
    bool importBattleConfigs(const std::string& filename);
    
    // Statistics and analysis
    void printStats() const;
    void printFinalAnalysis() const;
    void exportFinalAnalysis(const std::string& filename) const;
    void resetAllResults();
    
    // Access methods
    const std::vector<BattleConfig>& getBattleConfigs() const { return battleConfigs; }
    const EquipmentLoader& getEquipmentLoader() const { return equipmentLoader; }
    EquipmentLoader& getMutableEquipmentLoader() { return equipmentLoader; }
    
    // Direct equipment access for external systems
    const std::vector<SimpleWeapon>& getWeapons() const { return equipmentLoader.getWeapons(); }
    const std::vector<SimpleArmor>& getArmor() const { return equipmentLoader.getArmor(); }
    std::vector<SimpleWeapon>& getMutableWeapons() { return equipmentLoader.getMutableWeapons(); }
    std::vector<SimpleArmor>& getMutableArmor() { return equipmentLoader.getMutableArmor(); }
};

#endif // TEAM_CONFIGURATION_H
