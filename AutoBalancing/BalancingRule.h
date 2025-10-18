#ifndef BALANCINGRULE_H
#define BALANCINGRULE_H

#include <array>

#include "../Element/Stat.h"

const std::array<Stat,5> ChromosomeRules_Alpha= {{
    Stat(1, 0, 0, 0, 0),
    Stat(0, 1, 0, 0, 0),
    Stat(0, 0, 1, 0, 0),
    Stat(0, 0, 0, 1, 0),
    Stat(0, 0, 0, 0, 1)
}};

const std::array<Stat,20> ChromosomeRules_Beta= {{
    //Attack 
    Stat(1, -1, 0, 0, 0),
    Stat(1, 0, -1, 0, 0),
    Stat(1, 0, 0, -1, 0),
    Stat(1, 0, 0, 0, -1),
    //Defense
    Stat(-1, 1, 0, 0, 0),
    Stat(0, 1, -1, 0, 0),
    Stat(0, 1, 0, -1, 0),
    Stat(0, 1, 0, 0, -1),
    //HP
    Stat(-1, 0, 1, 0, 0),
    Stat(0, -1, 1, 0, 0),
    Stat(0, 0, 1, -1, 0),
    Stat(0, 0, 1, 0, -1),
    //Speed
    Stat(-1, 0, 0, 1, 0),
    Stat(0, -1, 0, 1, 0),
    Stat(0, 0, -1, 1, 0),
    Stat(0, 0, 0, 1, -1),
    //Threat
    Stat(-1, 0, 0, 0, 1),
    Stat(0, -1, 0, 0, 1),
    Stat(0, 0, -1, 0, 1),
    Stat(0, 0, 0, -1, 1)
}};

#endif