#ifndef GAMECOMPONENT_TO_CSV_H
#define GAMECOMPONENT_TO_CSV_H

#include <vector>

#include "../Simulator/TestSubject.h"

class GameComponentToCSV {
  public:
    static void Convert(std::vector<TestSubject *> testSubjects);
};

#endif