#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>

#include "../Element/Stat.h"
#include "../Element/ElementList.h"

#include "../Simulator/TestSubject.h"

class Chromosome
{
private: 

float rootMeanSquareError;

float degreeOfChange;

float fitness;

public:

std::vector<Stat> appliedStats;

std::vector<float> averageWinrate;

//First Chromosome
Chromosome(float RMSE) :rootMeanSquareError(RMSE) {};

Chromosome(std::vector<Stat> _appliedStats) : appliedStats(_appliedStats) {};

void ApplyStatToComponents(ElementList* componentList);

void Set_RMSE(float RMSE) {rootMeanSquareError = RMSE;}

void Set_DegreeOfChange(float value) {degreeOfChange = value;}

void Set_Fitness() {fitness = rootMeanSquareError + degreeOfChange;}

};

#endif