#include "GameCoponentToCSV.h"
#include "../Paths.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// Static variable definitions
std::string GameComponentToCSV::sharedRunDirectory = "";
int GameComponentToCSV::currentExperimentNumber = 1;

void GameComponentToCSV::Convert(std::vector<TestSubject *> testSubjects) {
    // Debug output
    std::cout << "GameComponentToCSV::Convert - Current experiment number: " << currentExperimentNumber << std::endl;
    std::cout << "GameComponentToCSV::Convert - Shared directory: " << sharedRunDirectory << std::endl;

    // Use the shared run directory and create experiment subdirectory
    std::string outputDir = sharedRunDirectory + "Experiment_" + std::to_string(currentExperimentNumber) + "\\";
    std::filesystem::create_directories(outputDir);

    std::cout << "Using run directory: " << outputDir << std::endl;

    // Generate filename in the new subdirectory
    std::string filename = outputDir + "GameComponents.csv";

    // Create output file
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create " << filename << " file!" << std::endl;
        return;
    }

    std::cout << "Creating file: " << filename << std::endl;

    // Check if we have any data
    if (testSubjects.empty()) {
        std::cerr << "Warning: No test subject data found!" << std::endl;
        csvFile.close();
        return;
    }

    // Write CSV header
    csvFile << "ID,Attack,Defense,Hitpoint,Speed,Threat" << std::endl;

    // Write data for each test subject
    for (const TestSubject *subject : testSubjects) {
        if (subject) {
            csvFile << subject->GetID();

            // Check if correctionStat exists and use it, otherwise use the regular stat
            if (subject->correctionStat != nullptr) {
                csvFile << "," << subject->correctionStat->GetAttack() << "," << subject->correctionStat->GetDefense()
                        << "," << subject->correctionStat->GetHP() << "," << subject->correctionStat->GetSpeed() << ","
                        << subject->correctionStat->GetThreat();
            } else {
                // Use regular stats if correctionStat is not available
                Stat &regularStat = const_cast<TestSubject *>(subject)->GetStat();
                csvFile << "," << regularStat.GetAttack() << "," << regularStat.GetDefense() << ","
                        << regularStat.GetHP() << "," << regularStat.GetSpeed() << "," << regularStat.GetThreat();
            }
            csvFile << std::endl;
        }
    }

    csvFile.close();
    std::cout << "GameComponents CSV file created successfully: " << filename << std::endl;
    std::cout << "Total test subjects processed: " << testSubjects.size() << std::endl;
}

// Set the shared run directory
void GameComponentToCSV::SetSharedDirectory(const std::string &sharedDir) {
    sharedRunDirectory = sharedDir;
}

// Set the current experiment number
void GameComponentToCSV::SetExperimentNumber(int experimentNumber) {
    currentExperimentNumber = experimentNumber;
}