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
};

#endif 
