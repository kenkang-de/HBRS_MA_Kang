#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "../Element/Stat.h"
#include "../Element/CounterTypeInitializer.h"
#include "TestSubject.h"



class Armor : public TestSubject {
    std::string name;
    Stat stat;
 
    CounterType armorType;
    std::vector<std::string> unitSynergyIDs;

public:
    Armor(std::string id, std::string name, Stat stat, CounterType armorType = CounterType::None) : 
    TestSubject(id), name(name), stat(stat), armorType(armorType) {}
    
    const std::string& GetName() const { return name; }
    Stat& GetStat() override{ return stat; }
    const Stat& GetStat() const { return stat; }

    void SetArmorType(CounterType armorType) {this->armorType = armorType;}
    CounterType GetArmorType() {return armorType;}

    std::vector<std::string>& GetUnitSynergyIDs() {return unitSynergyIDs;}
};

#endif