#include "GameCoponentToCSV.h"
#include "../Paths.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string GameComponentToCSV::experimentPath = "";

void GameComponentToCSV::Convert(std::vector<TestSubject *> testSubjects) {
    std::string filename = experimentPath + "\\GameComponents.csv";

    // Create output file
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        return;
    }

    if (testSubjects.empty()) {
        csvFile.close();
        return;
    }

    csvFile << "ID,Attack,Defense,Hitpoint,Speed,Threat,UsageCount,WinRate" << std::endl;

    for (const TestSubject *subject : testSubjects) {
        if (subject) {
            csvFile << subject->GetID();

            if (subject->correctionStat != nullptr) {
                csvFile << "," << subject->correctionStat->GetAttack() << "," << subject->correctionStat->GetDefense()
                        << "," << subject->correctionStat->GetHP() << "," << subject->correctionStat->GetSpeed() << ","
                        << subject->correctionStat->GetThreat();
            } else {
                Stat &regularStat = const_cast<TestSubject *>(subject)->GetStat();
                csvFile << "," << regularStat.GetAttack() << "," << regularStat.GetDefense() << ","
                        << regularStat.GetHP() << "," << regularStat.GetSpeed() << "," << regularStat.GetThreat();
            }

            // Add simulation statistics
            csvFile << "," << subject->UsageCount << "," << std::fixed << std::setprecision(4) << subject->WinRate;

            csvFile << std::endl;
        }
    }

    csvFile.close();
}

void GameComponentToCSV::SetExperimentPath(const std::string &path) {
    experimentPath = path;
}

std::string GameComponentToCSV::GetExperimentPath() {
    return experimentPath;
}