#include "BalancingLogToCSV.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void BalancingLogToCSV::Convert()
{
    // Create output file
    std::ofstream csvFile("BalancingLogResults.csv");
    
    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create BalancingLogResults.csv file!" << std::endl;
        return;
    }
    
    // Check if we have any data
    if (BalancingLog::BalancingLogs.empty()) {
        std::cerr << "Warning: No balancing log data found!" << std::endl;
        csvFile.close();
        return;
    }
    
    // Find the maximum number of entries in any generation for column headers
    size_t maxEntries = 0;
    for (const auto& generation : BalancingLog::BalancingLogs) {
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
        
        const auto& generationLogs = BalancingLog::BalancingLogs[gen];
        
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
    std::cout << "BalancingLog CSV file created successfully!" << std::endl;
}