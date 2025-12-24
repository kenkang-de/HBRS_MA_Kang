#ifndef GAMECOMPONENT_TO_CSV_H
#define GAMECOMPONENT_TO_CSV_H

#include <string>
#include <vector>

#include "../Simulator/TestSubject.h"

class GameComponentToCSV {
  public:
    static void Convert(std::vector<TestSubject *> testSubjects);
    static void SetExperimentPath(const std::string &path);
    static std::string GetExperimentPath();

  private:
    static std::string experimentPath;
};

#endif