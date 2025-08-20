#include "Unit.h"
#include <list>
#include <iostream>
#include "Team.h"

std::list<Unit> GenerateUnits(int amount)
{
    std::list<Unit> units;

    for (int i = 0; i < amount; i++)
    {
        Unit unit("Name", "ID");
        units.push_back(std::move(unit)); 
    }

    return units;
}

