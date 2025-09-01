#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "Stat.h"
#include "TestSubject.h"

class Armor : public TestSubject {
    std::string name;
    Stat stat;
public:
    Armor(std::string id, std::string name, Stat stat) : TestSubject(id), name(name), stat(stat) {}

    Armor() : TestSubject(""), name("Default Armor"), stat(Stat::Empty()) {} 
    
    const std::string& GetName() const { return name; }
    Stat& GetStat() { return stat; }
const Stat& GetStat() const { return stat; }
};

#endif