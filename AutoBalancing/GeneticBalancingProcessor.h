#ifndef GENETIC_BALANCING_PROCESSOR_H
#define GENETIC_BALANCING_PROCESSOR_H

#include <algorithm>
#include <array>
#include <iostream>
#include <random>

#include "BalancingLog.h"
#include "BalancingRule.h"
#include "Chromosome.h"

#include "../Element/ElementList.h"

#include "../Sampling/Batch.h"
#include "../Sampling/BatchCreator.h"

#include "../Simulator/Armor.h"
#include "../Simulator/Constants.h"
#include "../Simulator/Simulator.h"
#include "../Simulator/TestSubject.h"
#include "../Simulator/Weapon.h"

#include "../Analysis/RMSE.h"

class GeneticBalancingProcessor {

  public:
    static Chromosome *firstChromosome;

    static std::vector<Stat> firstStats;

    static float firstMagnitude;

    static int Generation;

    std::vector<Chromosome *> currentGenChromosomeList;

    std::vector<Chromosome *> nextGenChromosomeList;

    void GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject *> firstSubjects);

    int CalcStatMagnitude(std::vector<Stat> stats);
    float CosineSimilarity(std::vector<Stat> targetStats);

    std::vector<float> GetAverageWinrate(std::vector<TestSubject *> testSubjects);

    std::vector<Chromosome *> Instantiate_FirstGenChromosomes();

    std::vector<Stat> EmptyStats(int componentAmount);
    std::vector<TestSubject *> CombineToTestSubject(std::vector<Weapon> &weapons, std::vector<Armor> &armors);

    int BuffOrNerf(float winRate);

    std::vector<Stat> GetCorrectedStats(std::vector<TestSubject *> mergedTestSubjects);

    std::array<Chromosome *, 2> Pick2RandomLanes(std::vector<Chromosome *> chromosomeList);

    std::array<Chromosome *, 2> ParentTournament(std::vector<Chromosome *> ParentTournament);

    Chromosome *GetHighestFitnessChromosome(std::vector<Chromosome *> chromosomes);
    Chromosome *GetHighestFitnessChromosome(std::array<Chromosome *, 2> &chromosomes);
    Chromosome *GetLowestFitnessChromosome(std::vector<Chromosome *> chromosomes);
    Chromosome *GetLowestFitnessChromosome(std::array<Chromosome *, 2> &chromosomes);

    void SimulateChromosome(Simulator *simulator, std::vector<Batch> *batches, Chromosome *chromosome,
                            std::vector<TestSubject *> mergedTestSubjects);
    void RunAutoBalancing(Simulator *simulator, BatchConfig *batchConfig);
};

#endif
