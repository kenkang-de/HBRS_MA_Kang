#include "ElementList.h"
#include <algorithm>

 Weapon* ElementList::FindWeaponByID(std::string& weaponId)  {
    auto it = std::find_if(weapons.begin(), weapons.end(),
        [&weaponId]( Weapon& weapon) {
            return weapon.GetID() == weaponId;
        });
    
    return (it != weapons.end()) ? &(*it) : nullptr;
}

 Armor* ElementList::FindArmorByID(std::string& armorId)  {
    auto it = std::find_if(armors.begin(), armors.end(),
        [&armorId]( Armor& armor) {
            return armor.GetID() == armorId;
        });
    
    return (it != armors.end()) ? &(*it) : nullptr;
}