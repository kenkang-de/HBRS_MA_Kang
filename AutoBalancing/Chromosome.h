#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>

#include "../Element/ElementList.h"
#include "../Element/Stat.h"

#include "../Simulator/Constants.h"
#include "../Simulator/TestSubject.h"

class Chromosome {
  private:
    float rootMeanSquareError;

    float degreeOfChange;

    float fitness;

    bool isAppliedAll;

    bool survived = false;

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

    void ApplyStatToComponents(ElementList *componentList);

    float Get_Fitness() {
        return fitness;
    }
    float Get_MRSE() {
        return rootMeanSquareError;
    }
    float Get_DOG() {
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
    bool Get_Survived() {
        return survived;
    }
    void Set_Survived(bool value) {
        survived = value;
    }

    bool IsAppliedAll() {
        return isAppliedAll;
    }
};

#endif