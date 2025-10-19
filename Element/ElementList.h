#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <vector>
#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"

class ElementList
{
  public :
  std::vector<Weapon> weapons;
  std::vector<Armor> armors;  

  std::vector<Weapon>& getWeapons() { return weapons; }     
  std::vector<Armor>& getArmors() { return armors; }  

  Weapon* FindWeaponByID(const std::string& weaponId);
  Armor* FindArmorByID(const std::string& armorId);
};

#endif 
