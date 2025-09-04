#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "TestSubject.h"
#include "Stat.h"
#include <string>
#include <vector>

// Base equipment class with core functionality
class Equipment : public TestSubject {
protected:
    std::string id;
    std::string name;
    std::vector<Stat> stats;

public:
    Equipment(const std::string& equipmentId, const std::string& equipmentName)
        : id(equipmentId), name(equipmentName) {}
    
    virtual ~Equipment() = default;
    
    // Core getters
    const std::string& GetID() const { return id; }
    const std::string& GetName() const { return name; }
    const std::vector<Stat>& GetStats() const { return stats; }
    
    // Stat management
    void AddStat(const Stat& stat) { stats.push_back(stat); }
    void ClearStats() { stats.clear(); }
    
    // Find specific stat by type (simplified approach)
    const Stat* FindStat(const std::string& statName) const {
        // Since Stat class doesn't have names, just return the first stat if any exist
        if (!stats.empty()) {
            return &stats[0];
        }
        return nullptr;
    }
};

// Lightweight equipment classes for simulation
class SimpleWeapon : public Equipment {
public:
    SimpleWeapon(const std::string& weaponId, const std::string& weaponName = "")
        : Equipment(weaponId, weaponName.empty() ? weaponId : weaponName) {}
    
    // Equipment type identifier
    std::string GetType() const { return "Weapon"; }
};

class SimpleArmor : public Equipment {
public:
    SimpleArmor(const std::string& armorId, const std::string& armorName = "")
        : Equipment(armorId, armorName.empty() ? armorId : armorName) {}
    
    // Equipment type identifier
    std::string GetType() const { return "Armor"; }
};

#endif // EQUIPMENT_H
