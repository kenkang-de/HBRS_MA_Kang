#include "TestRunner.h"
#include "../Paths.h"
#include "../Analysis/ElementAnalysis.h"
#include "../Simulator/Unit.h"
#include "../Simulator/UnitGenerator.h"
#include "../Simulator/TurnManager.h"
#include "../Simulator/EquipmentManager.h"
#include "../Simulator/Team.h"
#include "../Simulator/Battlefield.h"
#include "../Simulator/BattleActionLoader.h"
#include "../Simulator/BattleManager.h"
#include "../Simulator/Constants.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <list>

int main() {
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << std::endl;
    
    // Run Simulator.exe with configurable simulations parameter
    std::cout << "Running " << SIMULATION_COUNT << " simulations..." << std::endl;
    
    // Change to Simulator directory and call Simulator.exe with parameter
    std::string command = "cd ../Simulator && .\\Simulator.exe " + std::to_string(SIMULATION_COUNT) + " > ..\\Sampling\\temp_simulation_output.txt 2>&1";
    
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cout << "Error: Simulation failed!" << std::endl;
        return 1;
    }
    
    // Parse the output and extract equipment analysis
    std::ifstream outputFile("temp_simulation_output.txt");
    if (!outputFile.is_open()) {
        std::cout << "Error: Could not read simulation output!" << std::endl;
        return 1;
    }
    
    // Look for equipment analysis section and save to report
    std::ofstream reportFile("../Log/V1/ElementAnalysisV1_Report.csv");
    if (!reportFile.is_open()) {
        std::cout << "Error: Could not create analysis report!" << std::endl;
        return 1;
    }
    
    std::string line;
    bool inAnalysisSection = false;
    
    while (std::getline(outputFile, line)) {
        if (line == "EQUIPMENT_ANALYSIS_START") {
            inAnalysisSection = true;
            continue;
        }
        if (line == "EQUIPMENT_ANALYSIS_END") {
            break;
        }
        if (inAnalysisSection) {
            reportFile << line << std::endl;
        }
    }
    
    outputFile.close();
    reportFile.close();
    
    // Clean up temp file
    std::remove("temp_simulation_output.txt");
    
    std::cout << "Equipment analysis completed" << std::endl;
    
    // Calculate and display total execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Execution time: ";
    
    if (duration.count() >= 60000) { // More than 1 minute
        auto minutes = duration.count() / 60000;
        auto seconds = (duration.count() % 60000) / 1000.0;
        std::cout << minutes << "m " << std::fixed << std::setprecision(1) << seconds << "s";
    } else if (duration.count() >= 1000) { // More than 1 second
        auto seconds = duration.count() / 1000.0;
        std::cout << std::fixed << std::setprecision(2) << seconds << " seconds";
    } else { // Less than 1 second
        std::cout << duration.count() << " milliseconds";
    }
    
    std::cout << std::endl << std::endl;
    
    return 0;
}
