#ifndef SHARED_EQUIPMENT_MANAGER_H
#define SHARED_EQUIPMENT_MANAGER_H

#include "Weapon.h"
#include "Armor.h"
#include <vector>
#include <memory>
#include <unordered_map>

class SharedEquipmentManager {
private:
    static std::unique_ptr<SharedEquipmentManager> instance;
    static bool loaded;
    
    std::vector<Weapon> weapons;
    std::vector<Armor> armor;
    
    // Index maps for fast ID lookup
    std::unordered_map<std::string, size_t> weaponIdToIndex;
    std::unordered_map<std::string, size_t> armorIdToIndex;
    
    SharedEquipmentManager() = default;
    
public:
    // Singleton access
    static SharedEquipmentManager& getInstance();
    
    // Load equipment data once
    void loadEquipment();
    
    // Access methods (return const references for safety)
    const std::vector<Weapon>& getWeapons() const { return weapons; }
    const std::vector<Armor>& getArmor() const { return armor; }
    
    // Mutable access for statistics updates
    std::vector<Weapon>& getMutableWeapons() { return weapons; }
    std::vector<Armor>& getMutableArmor() { return armor; }
    
    // Get all equipment (for analysis)
    std::vector<Weapon>& getAllWeapons() { return weapons; }
    std::vector<Armor>& getAllArmor() { return armor; }
    
    // Fast lookup by ID
    const Weapon* getWeaponById(const std::string& id) const;
    const Armor* getArmorById(const std::string& id) const;
    
    // Mutable lookup for statistics updates
    Weapon* getMutableWeaponById(const std::string& id);
    Armor* getMutableArmorById(const std::string& id);
    
    // Get weapon/armor at index
    const Weapon& getWeapon(size_t index) const { return weapons[index]; }
    const Armor& getArmor(size_t index) const { return armor[index]; }
    
    // Size methods
    size_t getWeaponCount() const { return weapons.size(); }
    size_t getArmorCount() const { return armor.size(); }
    
    // Iterator support for ConfigurationGenerator
    auto weaponBegin() const { return weapons.begin(); }
    auto weaponEnd() const { return weapons.end(); }
    auto armorBegin() const { return armor.begin(); }
    auto armorEnd() const { return armor.end(); }
};

#endif
