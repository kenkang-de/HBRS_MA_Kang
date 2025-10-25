#include "BalancingLogToCSV.h"
#include "../Constants.h"
#include "../Paths.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

void BalancingLogToCSV::Convert() {
    // Create directory if it doesn't exist
    std::string outputDir = Paths::FromAnalysis::LOG_BALANCING_V1_DIR;
    std::filesystem::create_directories(outputDir);

    // Generate unique filename in the specified directory
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

    // Add constants section at the end
    csvFile << std::endl; // Empty line separator
    csvFile << "Configuration Constants" << std::endl;
    csvFile << "UNITS_PER_TEAM," << UNITS_PER_TEAM << std::endl;
    csvFile << "TEST_TICK," << TEST_TICK << std::endl;
    csvFile << "SIMULATION_COUNT," << SIMULATION_COUNT << std::endl;
    csvFile << "DELAY_MULTIPLIER," << DELAY_MULTIPLIER << std::endl;
    csvFile << "MULTIPLIER_COUNTER," << MULTIPLIER_COUNTER << std::endl;
    csvFile << "MULTIPLIER_BASIC," << MULTIPLIER_BASIC << std::endl;
    csvFile << "DEFENSE_RATIO," << DEFENSE_RATIO << std::endl;
    csvFile << "SPEED_RATIO," << SPEED_RATIO << std::endl;
    csvFile << "APPLIEDSTAT_RANGE," << APPLIEDSTAT_RANGE << std::endl;
    csvFile << "INDIVIDUALS_PER_GENERATION," << INDIVIDUALS_PER_GENERATION << std::endl;
    csvFile << "CROSSOVER_PROBABILITY," << CROSSOVER_PROBABILITY << std::endl;
    csvFile << "MUTATION_PROBABILITY," << MUTATION_PROBABILITY << std::endl;
    csvFile << "MUTATION_SIGMA," << MUTATION_SIGMA << std::endl;
    csvFile << "TARGET_WINRATE," << TARGET_WINRATE << std::endl;
    csvFile << "TARGET_THRESHOLD," << TARGET_THRESHOLD << std::endl;
    csvFile << "FITNESS_THRESHOLD," << FITNESS_THRESHOLD << std::endl;
    csvFile << "FITNESS_MAX," << FITNESS_MAX << std::endl;
    csvFile << "RMSE_WEIGHT," << RMSE_WEIGHT << std::endl;
    csvFile << "DOC_WEIGHT," << DOC_WEIGHT << std::endl;
    csvFile << "MAXGENERATION," << MAXGENERATION << std::endl;

    csvFile.close();
    std::cout << "BalancingLog CSV file created successfully: " << filename << std::endl;
}