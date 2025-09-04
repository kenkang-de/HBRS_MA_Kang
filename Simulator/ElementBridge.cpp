#include "ElementBridge.h"
#include "EquipmentManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Weapon> ElementBridge::convertToSimulatorWeapons(const std::vector<SimpleWeapon>& elementWeapons, const std::unordered_map<std::string, BattleAction>& actionMap) {
    std::vector<Weapon> simulatorWeapons;
    
    for (const auto& elementWeapon : elementWeapons) {
        // Convert Element weapon to Simulator weapon
        const std::string& id = elementWeapon.GetID();
        const std::string& name = elementWeapon.GetName();
        
        // For now, create default stats - this should be improved to read actual stats
        Stat weaponStat(10, 2, 50, 5, 3); // Default values, should be loaded from Element data
        
        // Look up action
        BattleAction action;
        auto it = actionMap.find("A00"); // Default action, should be loaded from Element data
        if (it != actionMap.end()) {
            action = it->second;
        }
        
        Weapon simulatorWeapon(id, name, weaponStat, action);
        simulatorWeapons.push_back(simulatorWeapon);
    }
    
    return simulatorWeapons;
}

std::vector<Armor> ElementBridge::convertToSimulatorArmor(const std::vector<SimpleArmor>& elementArmor) {
    std::vector<Armor> simulatorArmor;
    
    for (const auto& elementArmorItem : elementArmor) {
        const std::string& id = elementArmorItem.GetID();
        const std::string& name = elementArmorItem.GetName();
        
        // For now, create default stats - this should be improved
        Stat armorStat(2, 8, 30, 2, 1); // Default values
        
        Armor simulatorArmorItem(id, name, armorStat);
        simulatorArmor.push_back(simulatorArmorItem);
    }
    
    return simulatorArmor;
}

bool ElementBridge::loadEquipmentFromElementOutput(const std::string& weaponFile, const std::string& armorFile, const std::unordered_map<std::string, BattleAction>& actionMap) {
    // Clear existing equipment
    WeaponList.clear();
    ArmorList.clear();
    
    // Load weapons from Element output
    std::ifstream weaponFileStream(weaponFile);
    if (!weaponFileStream.is_open()) {
        std::cerr << "[ElementBridge] Could not open weapon file: " << weaponFile << std::endl;
        return false;
    }
    
    std::string line;
    std::getline(weaponFileStream, line); // Skip header
    
    while (std::getline(weaponFileStream, line)) {
        std::stringstream ss(line);
        std::string id, name, actionId;
        int atk, def, spd, thr, hp;
        
        if (std::getline(ss, id, ',') &&
            std::getline(ss, name, ',') &&
            ss >> atk && ss.ignore() &&
            ss >> def && ss.ignore() &&
            ss >> spd && ss.ignore() &&
            ss >> thr && ss.ignore() &&
            ss >> hp && ss.ignore() &&
            std::getline(ss, actionId)) {
            
            Stat weaponStat(atk, def, hp, spd, thr);
            
            BattleAction action;
            auto it = actionMap.find(actionId);
            if (it != actionMap.end()) {
                action = it->second;
            }
            
            Weapon weapon(id, name, weaponStat, action);
            WeaponList.push_back(weapon);
        }
    }
    weaponFileStream.close();
    
    // Load armor from Element output  
    std::ifstream armorFileStream(armorFile);
    if (!armorFileStream.is_open()) {
        std::cerr << "[ElementBridge] Could not open armor file: " << armorFile << std::endl;
        return false;
    }
    
    std::getline(armorFileStream, line); // Skip header
    
    while (std::getline(armorFileStream, line)) {
        std::stringstream ss(line);
        std::string id, name;
        int atk, def, spd, thr, hp;
        
        if (std::getline(ss, id, ',') &&
            std::getline(ss, name, ',') &&
            ss >> atk && ss.ignore() &&
            ss >> def && ss.ignore() &&
            ss >> spd && ss.ignore() &&
            ss >> thr && ss.ignore() &&
            ss >> hp) {
            
            Stat armorStat(atk, def, hp, spd, thr);
            Armor armor(id, name, armorStat);
            ArmorList.push_back(armor);
        }
    }
    armorFileStream.close();
    
    std::cout << "[ElementBridge] Loaded " << WeaponList.size() << " weapons and " << ArmorList.size() << " armor from Element output" << std::endl;
    
    return true;
}
