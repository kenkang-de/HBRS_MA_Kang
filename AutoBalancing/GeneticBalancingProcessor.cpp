#include "GeneticBalancingProcessor.h"

Chromosome* GeneticBalancingProcessor::firstChromosome= nullptr;

std::vector<Stat> GeneticBalancingProcessor::firstStats;

//[ORDER] always weapon first!
void GeneticBalancingProcessor::GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject*> firstSubjects)
{
std::vector<Stat> stats;

for(Weapon& weapon: elementList->weapons)
{
stats.push_back(weapon.GetStat());
}

for(Armor& armor: elementList->armors)
{
stats.push_back(armor.GetStat());
}

GeneticBalancingProcessor::firstStats = stats;

GeneticBalancingProcessor::firstChromosome = new Chromosome(RMSE::Calculate(firstSubjects));

    std::cout << "Chromosome created!" << std::endl;  // Add this

}
