#include "BalancingLogToCSV.h"
#include "../Constants.h"
#include "../ExperimentSettings.h"
#include "../Paths.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string BalancingLogToCSV::experimentPath = "";

void BalancingLogToCSV::Convert() {
    std::string filename = experimentPath + "\\BalancingLogResults.csv";

    // Create output file
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        return;
    }

    if (BalancingLog::BalancingLogs.empty()) {
        csvFile.close();
        return;
    }

    // Find the maximum number of entries in any generation for column headers
    size_t maxEntries = 0;
    for (const auto &generation : BalancingLog::BalancingLogs) {
        maxEntries = std::max(maxEntries, generation.size());
    }
    csvFile << "Generation";
    for (size_t i = 1; i <= maxEntries; ++i) {
        csvFile << ",Entry" << i;
    }
    csvFile << std::endl;

    // Write data for each generation
    for (size_t gen = 0; gen < BalancingLog::BalancingLogs.size(); ++gen) {
        csvFile << (gen + 1);

        const auto &generationLogs = BalancingLog::BalancingLogs[gen];
        for (size_t entry = 0; entry < maxEntries; ++entry) {
            csvFile << ",";
            if (entry < generationLogs.size()) {
                csvFile << generationLogs[entry];
            }
        }
        csvFile << std::endl;
    }

    csvFile.close();

    std::string configFilename = experimentPath + "\\Configuration.txt";
    std::ofstream configFile(configFilename);

    if (!configFile.is_open()) {
        return;
    }

    configFile << "Experiment Settings" << std::endl;
    configFile << "========================" << std::endl;
    configFile << "APPLIEDSTAT_RANGE=" << ExperimentSettings::APPLIEDSTAT_RANGE << std::endl;
    configFile << "INDIVIDUALS_PER_GENERATION=" << ExperimentSettings::INDIVIDUALS_PER_GENERATION << std::endl;
    configFile << "MAXGENERATION=" << ExperimentSettings::MAXGENERATION << std::endl;
    configFile << "ELITES_PER_GENERATION=" << ExperimentSettings::ELITES_PER_GENERATION << std::endl;
    configFile << "CROSSOVER_PROBABILITY=" << ExperimentSettings::CROSSOVER_PROBABILITY << std::endl;
    configFile << "MUTATION_PROBABILITY=" << ExperimentSettings::MUTATION_PROBABILITY << std::endl;
    configFile << "MUTATION_SIGMA=" << ExperimentSettings::MUTATION_SIGMA << std::endl;
    configFile << "RATIO_BS=" << ExperimentSettings::RATIO_BS << std::endl;
    configFile << "RATIO_CS=" << ExperimentSettings::RATIO_CS << std::endl;
    configFile << "RATIO_SYS=" << ExperimentSettings::RATIO_SYS << std::endl;

    configFile.close();
}

void BalancingLogToCSV::SetExperimentPath(const std::string &path) {
    experimentPath = path;
}

std::string BalancingLogToCSV::GetExperimentPath() {
    return experimentPath;
}