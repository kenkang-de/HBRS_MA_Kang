// Mutation.h
#ifndef MUTATION_H
#define MUTATION_H

#include "../Element/Stat.h"
#include "Chromosome.h"
#include <vector>

class Mutation {
  public:
    static void GaussianMutation(std::vector<Chromosome *> chromosomeList);

  private:
    static void MutateStatObject(Stat &stat);
};

#endif