#ifndef EQUIPMENT_LOADER_H
#define EQUIPMENT_LOADER_H

#include "Equipment.h"
#include "../Paths.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Centralized equipment loading system
class EquipmentLoader {
private:
    std::vector<SimpleWeapon> weapons;
    std::vector<SimpleArmor> armor;
    
    // Load equipment from CSV files
    void loadWeaponsFromCSV(const std::string& filepath);
    void loadArmorFromCSV(const std::string& filepath);
    
public:
    EquipmentLoader() = default;
    
    // Load all equipment from data files
    bool loadAllEquipment();
    
    // Access to loaded equipment
    const std::vector<SimpleWeapon>& getWeapons() const { return weapons; }
    const std::vector<SimpleArmor>& getArmor() const { return armor; }
    
    // Mutable access for statistics tracking
    std::vector<SimpleWeapon>& getMutableWeapons() { return weapons; }
    std::vector<SimpleArmor>& getMutableArmor() { return armor; }
    
    // Find equipment by ID
    const SimpleWeapon* findWeapon(const std::string& id) const;
    const SimpleArmor* findArmor(const std::string& id) const;
    SimpleWeapon* findMutableWeapon(const std::string& id);
    SimpleArmor* findMutableArmor(const std::string& id);
    
    // Statistics
    size_t getWeaponCount() const { return weapons.size(); }
    size_t getArmorCount() const { return armor.size(); }
    
    // Reset all battle results
    void resetAllResults();
    
    // Print equipment summary
    void printSummary() const;
};

#endif // EQUIPMENT_LOADER_H
