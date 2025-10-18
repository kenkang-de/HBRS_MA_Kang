#ifndef GENETIC_BALANCING_PROCESSOR_H
#define GENETIC_BALANCING_PROCESSOR_H

#include <iostream>

#include "Chromosome.h"
#include "BalancingRule.h"
#include "BalancingLane.h"

#include "../Element/ElementList.h"

#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"
#include "../Simulator/TestSubject.h"
#include "../Simulator/Constants.h"

#include "../Analysis/RMSE.h"



class GeneticBalancingProcessor
{

public: 
static Chromosome* firstChromosome; 

static std::vector<Stat> firstStats;

static float firstMagnitude;

std::vector<BalancingLane> lanes; 

void GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject*> firstSubjects);

void GenerateBalancingLane();

int CalcStatMagnitude(std::vector<Stat> stats);

float CosineSimilarity(std::vector<Stat> targetStats);

std::vector<float> GetAverageWinrate(std::vector<TestSubject*> firstSubjects);

void Init_AlphaBetaChromosome();

std::vector<Stat> EmptyStats(int componentAmount);

std::vector<Stat> CreateAppliedStats_ALPHA(Chromosome* chromosome, const Stat* ruleStat);

std::vector<Stat> CreateAppliedStats_BETA(Chromosome* chromosome, const Stat* ruleStat);

int BuffOrNerf(float winRate);

void RunAutoBalancing();

};

#endif
