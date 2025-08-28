#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "Stat.h"


class Armor {
    std::string id;
    std::string name;
    Stat stat;
public:
    Armor(std::string id, std::string name, Stat stat) : id(id), name(name), stat(stat) {}

    Armor() : id(""), name("Default Armor"), stat(Stat::Empty()) {} 
    
    const std::string& GetID() const { return id; }
    Stat& GetStat() { return stat; }
const Stat& GetStat() const { return stat; }
};

#endif