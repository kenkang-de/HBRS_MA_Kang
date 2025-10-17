#ifndef STANDARD_DEVIATION_H
#define STANDARD_DEVIATION_H

#include <vector>
#include <cmath>

#include "../Simulator/TestSubject.h"
#include "../Simulator/Constants.h"

class RMSE
{
    public:

    static float Calculate(std::vector<TestSubject*> testSubjects);
};

#endif