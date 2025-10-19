#ifndef GENETIC_BALANCING_PROCESSOR_H
#define GENETIC_BALANCING_PROCESSOR_H

#include <iostream>

#include "Chromosome.h"
#include "BalancingRule.h"
#include "BalancingLane.h"

#include "../Element/ElementList.h"

#include "../Sampling/Batch.h"

#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"
#include "../Simulator/TestSubject.h"
#include "../Simulator/Constants.h"
#include "../Simulator/Simulator.h"

#include "../Analysis/RMSE.h"



class GeneticBalancingProcessor
{

public: 
static Chromosome* firstChromosome; 

static std::vector<Stat> firstStats;

static float firstMagnitude;

static int Generation;

std::vector<BalancingLane> lanes; 

void GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject*> firstSubjects);

void GenerateBalancingLane();

int CalcStatMagnitude(std::vector<Stat> stats);

float CosineSimilarity(std::vector<Stat> targetStats);

std::vector<float> GetAverageWinrate(std::vector<TestSubject*> testSubjects);

void Init_AlphaBetaChromosome();

std::vector<Stat> EmptyStats(int componentAmount);

std::vector<Stat> CreateAppliedStats_ALPHA(Chromosome* chromosome, const Stat* ruleStat);

std::vector<Stat> CreateAppliedStats_BETA(Chromosome* chromosome, const Stat* ruleStat);

int BuffOrNerf(float winRate);

std::vector<Stat> GetCorrectedStats(std::vector<TestSubject*> mergedTestSubjects);

void RunAutoBalancing(Simulator* simulator, std::vector<Batch>* batches);

std::vector<TestSubject*> MergeToTestSubject(std::vector<Weapon>& weapons,std::vector<Armor>& armors);     

};

#endif
