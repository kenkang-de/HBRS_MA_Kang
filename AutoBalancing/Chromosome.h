#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>

#include "../Element/Stat.h"

class Chromosome
{
private: 

float rootMeanSquareError;

float degreeOfChange;

float Fitness;

public:

std::vector<Stat> appliedStats;

std::vector<float> averageWinrate;

//First Chromosome
Chromosome(float RMSE) :rootMeanSquareError(RMSE) {};

Chromosome(std::vector<Stat> appliedStats);

};

#endif