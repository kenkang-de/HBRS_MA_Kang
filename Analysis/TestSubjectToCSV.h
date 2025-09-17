#ifndef TESTSUB_TO_CSV_H
#define TESTSUB_TO_CSV_H

#include <vector>

#include "../Simulator/TestSubject.h"

class TestSubjectToCSV
{
    public:
    void Convert(std::vector<TestSubject*> testSubjects);
};

#endif
