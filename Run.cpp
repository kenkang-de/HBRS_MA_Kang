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
        
        // Calculate number of batches (assuming configs are divided into batches)
        int numBatches = (numBattleConfigs + 9) / 10; // Round up division for batches of 10
        std::cout << "Batches: " << numBatches << std::endl;
        
        // Final timing
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Total execution time: ";
        if (duration.count() >= 1000) {
            auto seconds = duration.count() / 1000.0;
            std::cout << std::fixed << std::setprecision(2) << seconds << " seconds";
        } else {
            std::cout << duration.count() << " milliseconds";
        }
        std::cout << std::endl;
        
        return 0;
    }
    
private:
    bool executeElementGeneration() {
        
        // Build Element.exe (suppress output)
        std::string buildCmd = "cd " + Paths::ELEMENT_DIR + " && .\\build.bat >nul 2>&1";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element build failed!" << std::endl;
            return false;
        }
    
        
        // Execute Element.exe to generate equipment and configurations (suppress output)
        std::string execCmd = "cd " + Paths::ELEMENT_DIR + " && .\\Element.exe " + std::to_string(numBattleConfigs) + " >nul 2>&1";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeSampling() {
        
        // Build SamplingController (suppress output)
        std::string buildCmd = "cd " + Paths::SAMPLING_DIR + " && .\\build_sampling.bat >nul 2>&1";
        int buildResult = std::system(buildCmd.c_str());
        if (buildResult != 0) {
            std::cerr << "    ERROR: SamplingController build failed with exit code: " << buildResult << std::endl;
            return false;
        }
        
        // Execute SamplingController to create batch configurations (suppress output)
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SamplingMain.exe >nul 2>&1";
        int execResult = std::system(execCmd.c_str());
        if (execResult != 0) {
            std::cerr << "    ERROR: SamplingController execution failed with exit code: " << execResult << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeBatchSimulations() {
        // Build Simulator (suppress output)
        std::string buildCmd = "cd " + Paths::SIMULATOR_DIR + " && .\\build.bat >nul 2>&1";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Simulator build failed!" << std::endl;
            return false;
        }
        
        
        // Build SimpleBatchRunner (suppress output)
        std::string buildRunnerCmd = "cd " + Paths::SAMPLING_DIR + " && g++ -std=c++17 SimpleBatchRunner.cpp -o SimpleBatchRunner.exe >nul 2>&1";
        if (std::system(buildRunnerCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SimpleBatchRunner build failed!" << std::endl;
            return false;
        }
        
        
        // Execute batch simulations (suppress output)
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SimpleBatchRunner.exe " + std::to_string(simulationsPerBatch) + " >nul 2>&1";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Batch simulation execution failed!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool executeTestSubjectAnalysis() {
        
        // Build EquipmentAggregator to analyze the real TestSubject data from Equipment objects (suppress output)
        std::string buildCmd = "cd " + Paths::ANALYSIS_DIR + " && g++ -std=c++17 EquipmentAggregator.cpp -o EquipmentAggregator.exe >nul 2>&1";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: EquipmentAggregator build failed!" << std::endl;
            return false;
        }
        
        // Execute EquipmentAggregator to get real accumulated TestSubject statistics (suppress output)
        std::string execCmd = "cd " + Paths::ANALYSIS_DIR + " && .\\EquipmentAggregator.exe >nul 2>&1";
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
