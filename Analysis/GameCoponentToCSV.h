#ifndef GAMECOMPONENT_TO_CSV_H
#define GAMECOMPONENT_TO_CSV_H

#include <string>
#include <vector>

#include "../Simulator/TestSubject.h"

class GameComponentToCSV {
  public:
    static void Convert(std::vector<TestSubject *> testSubjects);
    static void SetSharedDirectory(const std::string &sharedDir);
    static void SetExperimentNumber(int experimentNumber);

  private:
    static std::string sharedRunDirectory;
    static int currentExperimentNumber;
};

#endif