#include "SamplingController.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Equipment Sampling Controller ===" << std::endl;
    
    // Configuration parameters
    int totalTeams = 200;
    int teamsPerBatch = 10;
    int targetSimulations = 2000;
    
    if (argc > 1) totalTeams = std::atoi(argv[1]);
    if (argc > 2) teamsPerBatch = std::atoi(argv[2]);
    if (argc > 3) targetSimulations = std::atoi(argv[3]);
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Total Teams: " << totalTeams << std::endl;
    std::cout << "  Teams per Batch: " << teamsPerBatch << std::endl;
    std::cout << "  Target Simulations: " << targetSimulations << std::endl;
    
    // Initialize sampling controller
    SamplingController sampler(totalTeams, teamsPerBatch, targetSimulations);
    
    // Load team configurations from Element output
    std::cout << "\\n[Stage 1] Loading team configurations..." << std::endl;
    if (!sampler.loadTeamConfigurations("../Element/element_test_configs.csv")) {
        std::cerr << "Failed to load team configurations!" << std::endl;
        return 1;
    }
    
    // Distribute teams into batches with sampling strategy
    std::cout << "\\n[Stage 2] Distributing teams into batches..." << std::endl;
    if (!sampler.distributeToBatches()) {
        std::cerr << "Failed to distribute teams into batches!" << std::endl;
        return 2;
    }
    
    // Export batch configurations for simulation
    std::cout << "\\n[Stage 3] Exporting batch configurations..." << std::endl;
    if (!sampler.exportBatches("../Sampling/Batches")) {
        std::cerr << "Failed to export batch configurations!" << std::endl;
        return 3;
    }
    
    // Print sampling statistics
    sampler.printSamplingStats();
    
    std::cout << "\\n=== Sampling Complete ===" << std::endl;
    std::cout << "Batch files created in: ../Sampling/Batches/" << std::endl;
    std::cout << "Ready for tournament simulation execution" << std::endl;
    
    return 0;
}
