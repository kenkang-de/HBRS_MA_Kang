#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "Stat.h"


class Armor {
    std::string name;
    Stat stat;
public:
    Armor(std::string name, Stat stat) : name(name), stat(stat) {}

    Armor() : name("Default Armor"), stat(Stat::Empty()) {} 
    Stat& GetStat() { return stat; }
const Stat& GetStat() const { return stat; }
};

#endif