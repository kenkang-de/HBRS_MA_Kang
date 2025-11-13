#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>

#include "../Element/ElementList.h"
#include "../Element/Stat.h"

#include "../Constants.h"
#include "../Simulator/TestSubject.h"

class Chromosome {
  private:
    float rootMeanSquareError;

    float degreeOfChange;

    float fitness;

    bool isAppliedAll;

  public:
    // Applied all at once
    Stat appliedStat_ALL;
    // Applied individually
    std::vector<Stat> appliedStat_INDIVIDUAL;

    std::vector<float> averageWinrates;

    Chromosome() {};
    // First Chromosome
    Chromosome(float RMSE) : rootMeanSquareError(RMSE) {};
    // Applied all at once
    Chromosome(Stat _appliedStat_ALL) : appliedStat_ALL(_appliedStat_ALL) {
        isAppliedAll = true;
    }
    // Applied individually
    Chromosome(std::vector<Stat> _appliedStat_INDIVIDUAL) : appliedStat_INDIVIDUAL(_appliedStat_INDIVIDUAL) {
        isAppliedAll = false;
    }

    Chromosome(Chromosome *other)
        : appliedStat_INDIVIDUAL(other->appliedStat_INDIVIDUAL), averageWinrates(other->averageWinrates),
          rootMeanSquareError(other->rootMeanSquareError), degreeOfChange(other->degreeOfChange),
          fitness(other->fitness), isAppliedAll(other->isAppliedAll) {}

    void ApplyStatToComponents(ElementList *componentList);

    float Get_Fitness() {
        return fitness;
    }
    float Get_MRSE() {
        return rootMeanSquareError;
    }
    float Get_DOC() {
        return degreeOfChange;
    }

    void Set_RMSE(float RMSE) {
        rootMeanSquareError = RMSE * RMSE_WEIGHT;
    }
    void Set_DegreeOfChange(float value) {
        degreeOfChange = value * DOC_WEIGHT;
    }
    void Set_Fitness() {
        fitness = rootMeanSquareError + degreeOfChange;
    }

    bool IsAppliedAll() {
        return isAppliedAll;
    }
};

#endif