#include "Unit.h"
#include <list>
#include <iostream>
#include "Team.h"

std::array<Unit,10> GenerateUnits()
{
    std::array<Unit,10> units;

    for (int i = 0; i < units.size(); i++)
    {
        Unit unit;
        units[i] = std::move(unit);
    }

    return units;
}

