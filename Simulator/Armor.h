#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "../Element/Stat.h"
#include "TestSubject.h"

   enum ArmorType{
    None,
    Light,
    Medium,
    Heavy
};

class Armor : public TestSubject {
    std::string name;
    Stat stat;
 
    ArmorType armorType;

public:
    Armor(std::string id, std::string name, Stat stat, ArmorType armorType = ArmorType::None) : 
    TestSubject(id), name(name), stat(stat), armorType(armorType) {}
    
    const std::string& GetName() const { return name; }
    Stat& GetStat() { return stat; }
    const Stat& GetStat() const { return stat; }

    void SetArmorType(ArmorType armorType) {this->armorType = armorType;}
    ArmorType GetArmorType() {return armorType;}

};

#endif