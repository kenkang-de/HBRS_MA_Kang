#include "Crossover.h"

std::array<Chromosome *, 2> Crossover::SinglePointCrossOver(std::array<Chromosome *, 2> parentChromosome) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    Stat &stat1 = parentChromosome[0]->appliedStat_ALL;
    Stat &stat2 = parentChromosome[1]->appliedStat_ALL;

    // Single-point crossover on stat components (5 components: Attack, Defense, HP, Speed, Threat)
    std::uniform_int_distribution<int> dist(1, 4); // Crossover points 1-4 (between 5 components)
    int crossoverPoint = dist(gen);

    // Create temporary copies to perform swap
    Stat tempStat1 = stat1;
    Stat tempStat2 = stat2;

    // Perform crossover based on component position
    switch (crossoverPoint) {
    case 1: // After Attack
        stat1 = Stat(tempStat1.GetAttack(), tempStat2.GetDefense(), tempStat2.GetHP(), tempStat2.GetSpeed(),
                     tempStat2.GetThreat());
        stat2 = Stat(tempStat2.GetAttack(), tempStat1.GetDefense(), tempStat1.GetHP(), tempStat1.GetSpeed(),
                     tempStat1.GetThreat());
        break;
    case 2: // After Defense
        stat1 = Stat(tempStat1.GetAttack(), tempStat1.GetDefense(), tempStat2.GetHP(), tempStat2.GetSpeed(),
                     tempStat2.GetThreat());
        stat2 = Stat(tempStat2.GetAttack(), tempStat2.GetDefense(), tempStat1.GetHP(), tempStat1.GetSpeed(),
                     tempStat1.GetThreat());
        break;
    case 3: // After HP
        stat1 = Stat(tempStat1.GetAttack(), tempStat1.GetDefense(), tempStat1.GetHP(), tempStat2.GetSpeed(),
                     tempStat2.GetThreat());
        stat2 = Stat(tempStat2.GetAttack(), tempStat2.GetDefense(), tempStat2.GetHP(), tempStat1.GetSpeed(),
                     tempStat1.GetThreat());
        break;
    case 4: // After Speed
        stat1 = Stat(tempStat1.GetAttack(), tempStat1.GetDefense(), tempStat1.GetHP(), tempStat1.GetSpeed(),
                     tempStat2.GetThreat());
        stat2 = Stat(tempStat2.GetAttack(), tempStat2.GetDefense(), tempStat2.GetHP(), tempStat2.GetSpeed(),
                     tempStat1.GetThreat());
        break;
    }

    Chromosome *child1 = new Chromosome(stat1);
    Chromosome *child2 = new Chromosome(stat2);

    std::array<Chromosome *, 2> result = {child1, child2};
    return result;
}
