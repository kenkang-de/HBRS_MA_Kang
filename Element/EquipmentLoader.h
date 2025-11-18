#ifndef EQUIPMENT_LOADER_H
#define EQUIPMENT_LOADER_H

#include "../Paths.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../Simulator/BattleAction.h"
#include "ElementList.h"

class EquipmentLoader {
  private:
  public:
    ElementList equipment;

    void LoadArmorListFromCSV(const std::string &filepath);
    void LoadWeaponListFromCSV(const std::string &filepath,
                               const std::unordered_map<std::string, BattleAction> &actionMap);

    ElementList InstantiateElements(const std::unordered_map<std::string, BattleAction> &actionMap);
    ElementList InstantiateElements(const std::unordered_map<std::string, BattleAction> &actionMap,
                                    std::string weaponFile, std::string armorFile);
};

#endif
