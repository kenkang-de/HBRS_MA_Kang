#ifndef CROSSOVER_H
#define CROSSOVER_H

#include <array>
#include <random>

#include "Chromosome.h"

class Crossover {
  public:
    static std::array<Chromosome *, 2> SinglePointCrossOver(std::array<Chromosome *, 2> parentChromosome);
};

#endif