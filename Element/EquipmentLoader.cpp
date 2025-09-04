#include "EquipmentLoader.h"

void EquipmentLoader::loadWeaponsFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[EquipmentLoader] Error: Could not open weapon file: " << filepath << std::endl;
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string id, name;
        
        if (std::getline(ss, id, ',') && std::getline(ss, name, ',')) {
            weapons.emplace_back(id, name);
        }
    }
    
    std::cout << "[EquipmentLoader] Loaded " << weapons.size() << " weapons from " << filepath << std::endl;
}

void EquipmentLoader::loadArmorFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[EquipmentLoader] Error: Could not open armor file: " << filepath << std::endl;
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string id, name;
        
        if (std::getline(ss, id, ',') && std::getline(ss, name, ',')) {
            armor.emplace_back(id, name);
        }
    }
    
    std::cout << "[EquipmentLoader] Loaded " << armor.size() << " armor from " << filepath << std::endl;
}

bool EquipmentLoader::loadAllEquipment() {
    weapons.clear();
    armor.clear();
    
    // Load from the centralized data directory
    loadWeaponsFromCSV(Paths::FromElement::DATA_DIR + "Weapon_v1.csv");
    loadArmorFromCSV(Paths::FromElement::DATA_DIR + "Armor_v1.csv");
    
    if (weapons.empty() || armor.empty()) {
        std::cerr << "[EquipmentLoader] Error: Failed to load equipment data" << std::endl;
        return false;
    }
    
    std::cout << "[EquipmentLoader] Successfully loaded " << weapons.size() 
              << " weapons and " << armor.size() << " armor pieces" << std::endl;
    return true;
}

const SimpleWeapon* EquipmentLoader::findWeapon(const std::string& id) const {
    for (const auto& weapon : weapons) {
        if (weapon.GetID() == id) {
            return &weapon;
        }
    }
    return nullptr;
}

const SimpleArmor* EquipmentLoader::findArmor(const std::string& id) const {
    for (const auto& armorItem : armor) {
        if (armorItem.GetID() == id) {
            return &armorItem;
        }
    }
    return nullptr;
}

SimpleWeapon* EquipmentLoader::findMutableWeapon(const std::string& id) {
    for (auto& weapon : weapons) {
        if (weapon.GetID() == id) {
            return &weapon;
        }
    }
    return nullptr;
}

SimpleArmor* EquipmentLoader::findMutableArmor(const std::string& id) {
    for (auto& armorItem : armor) {
        if (armorItem.GetID() == id) {
            return &armorItem;
        }
    }
    return nullptr;
}

void EquipmentLoader::resetAllResults() {
    for (auto& weapon : weapons) {
        weapon.resetResults();
    }
    for (auto& armorItem : armor) {
        armorItem.resetResults();
    }
    std::cout << "[EquipmentLoader] All equipment battle results reset" << std::endl;
}

void EquipmentLoader::printSummary() const {
    std::cout << "\n=== Equipment Loader Summary ===" << std::endl;
    std::cout << "Weapons: " << weapons.size() << std::endl;
    std::cout << "Armor: " << armor.size() << std::endl;
    std::cout << "Total Equipment: " << (weapons.size() + armor.size()) << std::endl;
    
    // Count used equipment
    int usedWeapons = 0, usedArmor = 0;
    for (const auto& weapon : weapons) {
        if (weapon.UsageCount > 0) usedWeapons++;
    }
    for (const auto& armorItem : armor) {
        if (armorItem.UsageCount > 0) usedArmor++;
    }
    
    if (usedWeapons > 0 || usedArmor > 0) {
        std::cout << "Used Equipment - Weapons: " << usedWeapons 
                  << ", Armor: " << usedArmor << std::endl;
    }
}
