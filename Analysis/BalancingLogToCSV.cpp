#include "BalancingLogToCSV.h"
#include <algorithm>
#include <fstream>
#include <iostream>

void BalancingLogToCSV::Convert() {
    // Generate unique filename
    std::string baseFilename = "BalancingLogResults";
    std::string extension = ".csv";
    std::string filename = baseFilename + extension;

    // Check if file exists and generate unique name
    int counter = 1;
    std::ifstream testFile(filename);
    while (testFile.good()) {
        testFile.close();
        filename = baseFilename + "_" + std::to_string(counter) + extension;
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
}