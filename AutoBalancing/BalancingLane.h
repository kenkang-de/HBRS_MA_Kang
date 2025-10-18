#ifndef BALANCINGLANE_H
#define BALANCINGLANE_H

#include "../Element/Stat.h"
#include "Chromosome.h"

enum CHROMOSOMETYPE
{
    ALPHA,BETA
};


class BalancingLane
{
    private:
    CHROMOSOMETYPE chromosomeType;
    Chromosome* chromosome;

    public:

    const Stat* adjustment;

    BalancingLane(const Stat* _adjustment, CHROMOSOMETYPE _chromosomeType) : 
    adjustment(_adjustment), chromosomeType(_chromosomeType) 
    {};

    Chromosome* GetChromosome() {return chromosome;}
    void SetChromosome(Chromosome* _chromosome) {chromosome = _chromosome;}

    CHROMOSOMETYPE GetChromosomeType() { return chromosomeType; }
};


#endif