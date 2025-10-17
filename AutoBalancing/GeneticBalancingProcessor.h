#ifndef GENETIC_BALANCING_PROCESSOR_H
#define GENETIC_BALANCING_PROCESSOR_H

#include <iostream>

#include "Chromosome.h"

#include "../Element/ElementList.h"

#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"
#include "../Simulator/TestSubject.h"

#include "../Analysis/RMSE.h"



class GeneticBalancingProcessor
{

public: 
static Chromosome* firstChromosome; 

static std::vector<Stat> firstStats;

void GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject*> firstSubjects);


};

#endif
