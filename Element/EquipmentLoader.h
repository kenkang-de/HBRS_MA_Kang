#ifndef EQUIPMENT_LOADER_H
#define EQUIPMENT_LOADER_H

#include "../Paths.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ElementList.h"
#include "../Simulator/BattleAction.h"

class EquipmentLoader {
private:

    
public:
    ElementList equipment;

    void LoadArmorListFromCSV(const std::string& filepath);
    void LoadWeaponListFromCSV(const std::string& filepath, const std::unordered_map<std::string, BattleAction>& actionMap);

    ElementList InstantiateElements(const std::unordered_map<std::string,BattleAction>& actionMap);
};

#endif 
