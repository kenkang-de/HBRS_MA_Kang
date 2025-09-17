#include "TestSubjectToCSV.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void TestSubjectToCSV::Convert(std::vector<TestSubject*> testSubjects)
{
    // Create output file
    std::ofstream csvFile("TestSubjectResults.csv");
    
    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create CSV file!" << std::endl;
        return;
    }
    
    // Write CSV header
    csvFile << "ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // Write data for each TestSubject
    for (const TestSubject* subject : testSubjects) {
        if (subject) {
            csvFile << subject->GetID() << ","
                   << std::fixed << std::setprecision(4) << subject->WinRate << ","
                   << subject->TotalWin << ","
                   << subject->TotalLost << ","
                   << subject->TotalDraw << ","
                   << subject->UsageCount << std::endl;
        }
    }
    
    csvFile.close();
}