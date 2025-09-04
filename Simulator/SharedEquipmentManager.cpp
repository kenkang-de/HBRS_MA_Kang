#include "SharedEquipmentManager.h"
#include "EquipmentManager.h"
#include "BattleActionLoader.h"
#include <iostream>

// Static member definitions
std::unique_ptr<SharedEquipmentManager> SharedEquipmentManager::instance = nullptr;
bool SharedEquipmentManager::loaded = false;

SharedEquipmentManager& SharedEquipmentManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<SharedEquipmentManager>(new SharedEquipmentManager());
    }
    return *instance;
}

void SharedEquipmentManager::loadEquipment() {
    if (loaded) {
        return; // Already loaded
    }
    
    std::cout << "[SharedEquipmentManager] Loading equipment data..." << std::endl;
    
    // Load battle actions first
    auto actionMap = LoadActionsFromYAML("BattleActions.yaml");
    
    // Use existing EquipmentManager functions but populate our vectors
    LoadArmorListFromCSV("Data/Armor_v1.csv");
    LoadWeaponListFromCSV("Data/Weapon_v1.csv", actionMap);
    
    // Copy from global vectors to our local vectors
    extern std::vector<Armor> ArmorList;
    extern std::vector<Weapon> WeaponList;
    
    armor = ArmorList;
    weapons = WeaponList;
    
    // Build index maps for fast lookup
    for (size_t i = 0; i < weapons.size(); ++i) {
        weaponIdToIndex[weapons[i].GetID()] = i;
    }
    
    for (size_t i = 0; i < armor.size(); ++i) {
        armorIdToIndex[armor[i].GetID()] = i;
    }
    
    loaded = true;
    std::cout << "[SharedEquipmentManager] Loaded " << weapons.size() 
              << " weapons, " << armor.size() << " armor" << std::endl;
}

const Weapon* SharedEquipmentManager::getWeaponById(const std::string& id) const {
    auto it = weaponIdToIndex.find(id);
    if (it != weaponIdToIndex.end()) {
        return &weapons[it->second];
    }
    return nullptr;
}

const Armor* SharedEquipmentManager::getArmorById(const std::string& id) const {
    auto it = armorIdToIndex.find(id);
    if (it != armorIdToIndex.end()) {
        return &armor[it->second];
    }
    return nullptr;
}

Weapon* SharedEquipmentManager::getMutableWeaponById(const std::string& id) {
    auto it = weaponIdToIndex.find(id);
    if (it != weaponIdToIndex.end()) {
        return &weapons[it->second];
    }
    return nullptr;
}

Armor* SharedEquipmentManager::getMutableArmorById(const std::string& id) {
    auto it = armorIdToIndex.find(id);
    if (it != armorIdToIndex.end()) {
        return &armor[it->second];
    }
    return nullptr;
}
