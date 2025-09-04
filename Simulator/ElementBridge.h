#ifndef ELEMENT_BRIDGE_H
#define ELEMENT_BRIDGE_H

#include "Weapon.h"
#include "Armor.h"
#include "../Element/Equipment.h"
#include <vector>
#include <memory>

// Bridge to convert Element equipment to Simulator equipment
class ElementBridge {
public:
    // Convert Element equipment to Simulator equipment
    static std::vector<Weapon> convertToSimulatorWeapons(const std::vector<SimpleWeapon>& elementWeapons, const std::unordered_map<std::string, BattleAction>& actionMap);
    static std::vector<Armor> convertToSimulatorArmor(const std::vector<SimpleArmor>& elementArmor);
    
    // Load equipment from Element-generated files
    static bool loadEquipmentFromElementOutput(const std::string& weaponFile, const std::string& armorFile, const std::unordered_map<std::string, BattleAction>& actionMap);
};

#endif
