#ifndef STANDARD_DEVIATION_H
#define STANDARD_DEVIATION_H

#include <algorithm>
#include <cmath>
#include <vector>

#include "../Constants.h"
#include "../Simulator/TestSubject.h"

class RMSE {
  public:
    static float Calculate(std::vector<TestSubject *> testSubjects);
};

#endif