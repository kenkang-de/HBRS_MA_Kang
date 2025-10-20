#include "BalancingLane.h"
#include "Chromosome.h"

void BalancingLane::SetChromosome(Chromosome* _chromosome) 
{
    chromosome = _chromosome;
    if (_chromosome != nullptr) {
        _chromosome->Set_BalancingLane(this);
    }
}