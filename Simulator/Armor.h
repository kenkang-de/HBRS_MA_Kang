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

public:
    Armor(std::string id, std::string name, Stat stat, CounterType armorType = CounterType::None) : 
    TestSubject(id), name(name), stat(stat), armorType(armorType) {}
    
    const std::string& GetName() const { return name; }
    Stat& GetStat() { return stat; }
    const Stat& GetStat() const { return stat; }

    void SetArmorType(CounterType armorType) {this->armorType = armorType;}
    CounterType GetArmorType() {return armorType;}

};

#endif