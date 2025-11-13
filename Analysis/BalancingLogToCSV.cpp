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

void BalancingLogToCSV::Convert() {
    // Create base output directory if it doesn't exist
    std::string baseOutputDir = Paths::FromAnalysis::LOG_BALANCING_V1_DIR;
    std::filesystem::create_directories(baseOutputDir);

    // Create timestamp-based subdirectory
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    std::ostringstream timestampStream;
    timestampStream << std::put_time(&tm, "%Y%m%d_%H%M%S");
    std::string timestamp = timestampStream.str();

    std::string outputDir = baseOutputDir + "Run_" + timestamp + "\\";
    std::filesystem::create_directories(outputDir);

    std::cout << "Created new run directory: " << outputDir << std::endl;

    // Generate unique filename in the new subdirectory
    std::string baseFilename = "BalancingLogResults";
    std::string extension = ".csv";
    std::string filename = outputDir + baseFilename + extension;

    // Check if file exists and generate unique name
    int counter = 1;
    std::ifstream testFile(filename);
    while (testFile.good()) {
        testFile.close();
        filename = outputDir + baseFilename + "_" + std::to_string(counter) + extension;
        testFile.open(filename);
        counter++;
    }
    testFile.close();

    // Create output file with unique name
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create " << filename << " file!" << std::endl;
        return;
    }

    std::cout << "Creating file: " << filename << std::endl;

    // Check if we have any data
    if (BalancingLog::BalancingLogs.empty()) {
        std::cerr << "Warning: No balancing log data found!" << std::endl;
        csvFile.close();
        return;
    }

    // Find the maximum number of entries in any generation for column headers
    size_t maxEntries = 0;
    for (const auto &generation : BalancingLog::BalancingLogs) {
        maxEntries = std::max(maxEntries, generation.size());
    }

    std::cout << "Total generations: " << BalancingLog::BalancingLogs.size() << std::endl;
    std::cout << "Max entries per generation: " << maxEntries << std::endl;

    // Write CSV header
    csvFile << "Generation";
    for (size_t i = 1; i <= maxEntries; ++i) {
        csvFile << ",Entry" << i;
    }
    csvFile << std::endl;

    // Write data for each generation
    for (size_t gen = 0; gen < BalancingLog::BalancingLogs.size(); ++gen) {
        // Write generation number (1-based)
        csvFile << (gen + 1);

        const auto &generationLogs = BalancingLog::BalancingLogs[gen];

        // Debug output
        std::cout << "Generation " << (gen + 1) << " has " << generationLogs.size() << " entries" << std::endl;

        // Write all entries for this generation
        for (size_t entry = 0; entry < maxEntries; ++entry) {
            csvFile << ",";
            if (entry < generationLogs.size()) {
                csvFile << generationLogs[entry];
            }
            // If no entry exists, leave empty (just the comma)
        }
        csvFile << std::endl;
    }

    csvFile.close();
    std::cout << "BalancingLog CSV file created successfully: " << filename << std::endl;

    // Create separate configuration constants file
    std::string configFilename = outputDir + "Configuration.txt";
    std::ofstream configFile(configFilename);

    if (!configFile.is_open()) {
        std::cerr << "Error: Could not create " << configFilename << " file!" << std::endl;
        return;
    }

    configFile << "Experiment Settings" << std::endl;
    configFile << "========================" << std::endl;
    // configFile << "DELAY_MULTIPLIER: " << DELAY_MULTIPLIER << std::endl;
    // configFile << "MULTIPLIER_COUNTER: " << MULTIPLIER_COUNTER << std::endl;
    // configFile << "MULTIPLIER_BASIC: " << MULTIPLIER_BASIC << std::endl;
    // configFile << "DEFENSE_RATIO: " << DEFENSE_RATIO << std::endl;
    // configFile << "SPEED_RATIO: " << SPEED_RATIO << std::endl;
    configFile << "APPLIEDSTAT_RANGE: " << ExperimentSettings::APPLIEDSTAT_RANGE << std::endl;
    configFile << "INDIVIDUALS_PER_GENERATION: " << ExperimentSettings::INDIVIDUALS_PER_GENERATION << std::endl;
    configFile << "ELITES_PER_GENERATION: " << ExperimentSettings::ELITES_PER_GENERATION << std::endl;
    configFile << "CROSSOVER_PROBABILITY: " << ExperimentSettings::CROSSOVER_PROBABILITY << std::endl;
    configFile << "MUTATION_PROBABILITY: " << ExperimentSettings::MUTATION_PROBABILITY << std::endl;
    configFile << "MUTATION_SIGMA: " << ExperimentSettings::MUTATION_SIGMA << std::endl;
    // configFile << "TARGET_WINRATE: " << TARGET_WINRATE << std::endl;
    // configFile << "TARGET_THRESHOLD: " << TARGET_THRESHOLD << std::endl;
    // configFile << "FITNESS_THRESHOLD: " << FITNESS_THRESHOLD << std::endl;
    // configFile << "FITNESS_MAX: " << FITNESS_MAX << std::endl;
    // configFile << "RMSE_WEIGHT: " << RMSE_WEIGHT << std::endl;
    // configFile << "DOC_WEIGHT: " << DOC_WEIGHT << std::endl;
    configFile << "MAXGENERATION: " << ExperimentSettings::MAXGENERATION << std::endl;

    configFile.close();
    std::cout << "Configuration file created successfully: " << configFilename << std::endl;
}