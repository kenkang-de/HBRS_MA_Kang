#include "GameCoponentToCSV.h"
#include "../Paths.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void GameComponentToCSV::Convert(std::vector<TestSubject *> testSubjects) {
    // Create base output directory if it doesn't exist
    std::string baseOutputDir = Paths::FromAnalysis::LOG_BALANCING_V1_DIR;
    std::filesystem::create_directories(baseOutputDir);

    // Create timestamp-based subdirectory (same as BalancingLogToCSV)
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    std::ostringstream timestampStream;
    timestampStream << std::put_time(&tm, "%Y%m%d_%H%M%S");
    std::string timestamp = timestampStream.str();

    std::string outputDir = baseOutputDir + "Run_" + timestamp + "\\";
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