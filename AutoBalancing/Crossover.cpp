#include "Crossover.h"

std::array<Chromosome *, 2> Crossover::SinglePointCrossOver(std::array<Chromosome *, 2> parentChromosome) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    Chromosome *child1 = nullptr;
    Chromosome *child2 = nullptr;

    if (parentChromosome[0]->IsAppliedAll()) {
        // Original version: single stat crossover with component-level crossover
        Stat stat1 = parentChromosome[0]->appliedStat_ALL;
        Stat stat2 = parentChromosome[1]->appliedStat_ALL;

        // Create stat component arrays for cleaner crossover
        int components1[] = {stat1.GetAttack(), stat1.GetDefense(), stat1.GetHP(), stat1.GetSpeed(), stat1.GetThreat()};
        int components2[] = {stat2.GetAttack(), stat2.GetDefense(), stat2.GetHP(), stat2.GetSpeed(), stat2.GetThreat()};

        // Single-point crossover on components
        std::uniform_int_distribution<int> dist(1, 4);
        int crossoverPoint = dist(gen);

        // Swap components after crossover point
        for (int i = crossoverPoint; i < 5; ++i) {
            std::swap(components1[i], components2[i]);
        }

        // Create new stats from crossed components
        Stat childStat1(components1[0], components1[1], components1[2], components1[3], components1[4]);
        Stat childStat2(components2[0], components2[1], components2[2], components2[3], components2[4]);

        child1 = new Chromosome(childStat1);
        child2 = new Chromosome(childStat2);
    } else {
        // New version: individual stats crossover
        std::vector<Stat> &stats1 = parentChromosome[0]->appliedStat_INDIVIDUAL;
        std::vector<Stat> &stats2 = parentChromosome[1]->appliedStat_INDIVIDUAL;

        // Single-point crossover on the vector of stats
        std::uniform_int_distribution<int> dist(1, stats1.size() - 1); // Crossover points 1 to size-1
        int crossoverPoint = dist(gen);

        // Create copies for crossover
        std::vector<Stat> childStats1 = stats1;
        std::vector<Stat> childStats2 = stats2;

        // Perform single-point crossover: swap elements after crossover point
        for (size_t i = crossoverPoint; i < stats1.size(); ++i) {
            childStats1[i] = stats2[i];
            childStats2[i] = stats1[i];
        }

        child1 = new Chromosome(childStats1);
        child2 = new Chromosome(childStats2);
    }

    std::array<Chromosome *, 2> result = {child1, child2};
    return result;
}
