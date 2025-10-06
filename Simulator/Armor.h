#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "../Element/Stat.h"
#include "TestSubject.h"



class Armor : public TestSubject {
    std::string name;
    Stat stat;

    static enum ArmorType{
    Light,
    Medium,
    Heavy};

    ArmorType armorType;
public:
    Armor(std::string id, std::string name, Stat stat) : TestSubject(id), name(name), stat(stat) {}
    
    const std::string& GetName() const { return name; }
    Stat& GetStat() { return stat; }
    const Stat& GetStat() const { return stat; }
};

#endif