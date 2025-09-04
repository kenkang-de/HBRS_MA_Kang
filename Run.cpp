#include "Paths.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <filesystem>

class NewMasterController {
private:
    std::string configFile;
    int numBattleConfigs;
    int simulationsPerBatch;
    bool verboseOutput;
    
public:
    NewMasterController(const std::string& config = "master_config.txt") 
        : configFile(config), numBattleConfigs(100), simulationsPerBatch(1), verboseOutput(true) {}
    
    bool loadConfiguration() {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            std::cout << "[Master] Config file not found, using defaults..." << std::endl;
            std::cout << "[Master] Default: 100 battle configs (200 teams), 10 simulations per batch" << std::endl;
            return true;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;
            
            if (line.find("NUM_BATTLE_CONFIGS=") == 0) {
                numBattleConfigs = std::stoi(line.substr(19));
            } else if (line.find("SIMULATIONS_PER_BATCH=") == 0) {
                simulationsPerBatch = std::stoi(line.substr(22));
            } else if (line.find("VERBOSE_OUTPUT=") == 0) {
                verboseOutput = (line.substr(15) == "true");
            }
        }
        return true;
    }
    
    int executeFullPipeline() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Stage 1: Element Generation (Team Configuration)
        if (!executeElementGeneration()) {
            std::cerr << "ERROR: Element generation failed!" << std::endl;
            return 1;
        }
        
        // Stage 2: Sampling & Batch Creation
        if (!executeSampling()) {
            std::cerr << "ERROR: Sampling execution failed!" << std::endl;
            return 2;
        }
        
        // Stage 3: Batch Simulation Execution
        if (!executeBatchSimulations()) {
            std::cerr << "ERROR: Batch simulation execution failed!" << std::endl;
            return 3;
        }
        
        // Final timing and summary
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n=== Research Pipeline Execution Complete ===" << std::endl;
        std::cout << "Total execution time: ";
        
        if (duration.count() >= 60000) {
            auto minutes = duration.count() / 60000;
            auto seconds = (duration.count() % 60000) / 1000.0;
            std::cout << minutes << "m " << std::fixed << std::setprecision(1) << seconds << "s";
        } else if (duration.count() >= 1000) {
            auto seconds = duration.count() / 1000.0;
            std::cout << std::fixed << std::setprecision(2) << seconds << " seconds";
        } else {
            std::cout << duration.count() << " milliseconds";
        }
        
        return 0;
    }
    
private:
    bool executeElementGeneration() {
        
        // Build Element.exe
        std::string buildCmd = "cd " + Paths::ELEMENT_DIR + " && .\\build.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element build failed!" << std::endl;
            return false;
        }
    
        
        // Execute Element.exe to generate equipment and configurations
        std::string execCmd = "cd " + Paths::ELEMENT_DIR + " && .\\Element.exe " + std::to_string(numBattleConfigs);
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeSampling() {
        
        // Build SamplingController
        std::string buildCmd = "cd " + Paths::SAMPLING_DIR + " && .\\build_sampling.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SamplingController build failed!" << std::endl;
            return false;
        }
        
        // Execute SamplingController to create batch configurations
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SamplingMain.exe";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SamplingController execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeBatchSimulations() {
        // Build Simulator
        std::string buildCmd = "cd " + Paths::SIMULATOR_DIR + " && .\\build.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Simulator build failed!" << std::endl;
            return false;
        }
        
        
        // Build SimpleBatchRunner
        std::string buildRunnerCmd = "cd " + Paths::SAMPLING_DIR + " && g++ -std=c++17 SimpleBatchRunner.cpp -o SimpleBatchRunner.exe";
        if (std::system(buildRunnerCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SimpleBatchRunner build failed!" << std::endl;
            return false;
        }
        
        
        // Execute batch simulations
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SimpleBatchRunner.exe " + std::to_string(simulationsPerBatch);
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Batch simulation execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeTestSubjectAnalysis() {
        
        // Build EquipmentAggregator to analyze the real TestSubject data from Equipment objects
        std::string buildCmd = "cd " + Paths::ANALYSIS_DIR + " && g++ -std=c++17 EquipmentAggregator.cpp -o EquipmentAggregator.exe";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: EquipmentAggregator build failed!" << std::endl;
            return false;
        }
        
        // Execute EquipmentAggregator to get real accumulated TestSubject statistics
        std::string execCmd = "cd " + Paths::ANALYSIS_DIR + " && .\\EquipmentAggregator.exe";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: TestSubject analysis execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    std::string configFile = "master_config.txt";
    
    if (argc > 1) {
        configFile = argv[1];
    }
    
    NewMasterController controller(configFile);
    
    if (!controller.loadConfiguration()) {
        std::cerr << "Failed to load configuration!" << std::endl;
        return 1;
    }
    
    return controller.executeFullPipeline();
}
