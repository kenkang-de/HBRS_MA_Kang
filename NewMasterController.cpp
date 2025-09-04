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
        
        std::cout << "=== MA Research Pipeline Controller ===" << std::endl;
        std::cout << "Battle Configurations: " << numBattleConfigs << " (generates " << (numBattleConfigs * 2) << " teams)" << std::endl;
        std::cout << "Simulations per Batch: " << simulationsPerBatch << std::endl;
        std::cout << "Expected Batches: " << ((numBattleConfigs * 2) / 10) << " batches of 10 teams each" << std::endl;
        std::cout << "Expected Total Matches: " << ((numBattleConfigs * 2) / 10) * 45 << " matches" << std::endl;
        std::cout << std::endl;
        
        // Stage 1: Element Generation (Team Configuration)
        std::cout << "[Stage 1] Equipment & Team Configuration Generation..." << std::endl;
        if (!executeElementGeneration()) {
            std::cerr << "ERROR: Element generation failed!" << std::endl;
            return 1;
        }
        
        // Stage 2: Sampling & Batch Creation
        std::cout << "[Stage 2] Sampling & Batch Configuration..." << std::endl;
        if (!executeSampling()) {
            std::cerr << "ERROR: Sampling execution failed!" << std::endl;
            return 2;
        }
        
        // Stage 3: Batch Simulation Execution
        std::cout << "[Stage 3] Tournament Simulation Execution..." << std::endl;
        if (!executeBatchSimulations()) {
            std::cerr << "ERROR: Batch simulation execution failed!" << std::endl;
            return 3;
        }
        
        // Stage 4: TestSubject Equipment Analysis (using real accumulated Equipment data)
        std::cout << "[Stage 4] TestSubject Equipment Analysis..." << std::endl;
        if (!executeTestSubjectAnalysis()) {
            std::cerr << "ERROR: TestSubject analysis failed!" << std::endl;
            return 4;
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
        
        std::cout << std::endl;
        std::cout << "Configuration Files: " << Paths::ELEMENT_DIR + "element_test_configs.csv" << std::endl;
        std::cout << "Batch Files: " << Paths::SAMPLING_DIR + "Batches/" << std::endl;
        std::cout << "Simulation Results: " << Paths::LOG_V1_DIR << std::endl;
        std::cout << "Equipment Analysis: " << Paths::LOG_V1_DIR + "EquipmentAnalysis_Aggregated.csv" << std::endl;
        std::cout << "Analysis Reports: " << Paths::ANALYSIS_DIR << std::endl;
        
        return 0;
    }
    
private:
    bool executeElementGeneration() {
        if (verboseOutput) std::cout << "  → Building Element.exe..." << std::endl;
        
        // Build Element.exe
        std::string buildCmd = "cd " + Paths::ELEMENT_DIR + " && .\\build.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element build failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Generating " << numBattleConfigs << " battle configurations (" << (numBattleConfigs * 2) << " teams)..." << std::endl;
        
        // Execute Element.exe to generate equipment and configurations
        std::string execCmd = "cd " + Paths::ELEMENT_DIR + " && .\\Element.exe " + std::to_string(numBattleConfigs);
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Element execution failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Equipment and team configuration generation complete" << std::endl;
        return true;
    }
    
    bool executeSampling() {
        if (verboseOutput) std::cout << "  → Building SamplingController..." << std::endl;
        
        // Build SamplingController
        std::string buildCmd = "cd " + Paths::SAMPLING_DIR + " && .\\build_sampling.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SamplingController build failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Executing sampling and batch creation..." << std::endl;
        
        // Execute SamplingController to create batch configurations
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SamplingMain.exe";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SamplingController execution failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Batch configuration creation complete" << std::endl;
        return true;
    }
    
    bool executeBatchSimulations() {
        if (verboseOutput) std::cout << "  → Building Simulator.exe..." << std::endl;
        
        // Build Simulator
        std::string buildCmd = "cd " + Paths::SIMULATOR_DIR + " && .\\build.bat";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Simulator build failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Building SimpleBatchRunner..." << std::endl;
        
        // Build SimpleBatchRunner
        std::string buildRunnerCmd = "cd " + Paths::SAMPLING_DIR + " && g++ -std=c++17 SimpleBatchRunner.cpp -o SimpleBatchRunner.exe";
        if (std::system(buildRunnerCmd.c_str()) != 0) {
            std::cerr << "    ERROR: SimpleBatchRunner build failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → Running tournament simulations on all batches..." << std::endl;
        
        // Execute batch simulations
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\SimpleBatchRunner.exe " + std::to_string(simulationsPerBatch);
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Batch simulation execution failed!" << std::endl;
            return false;
        }
        
        if (verboseOutput) std::cout << "  → All batch simulations complete" << std::endl;
        return true;
    }
    
    bool executeTestSubjectAnalysis() {
        std::cout << "  → Building EquipmentAggregator for TestSubject analysis..." << std::endl;
        
        // Build EquipmentAggregator to analyze the real TestSubject data from Equipment objects
        std::string buildCmd = "cd " + Paths::ANALYSIS_DIR + " && g++ -std=c++17 EquipmentAggregator.cpp -o EquipmentAggregator.exe";
        if (std::system(buildCmd.c_str()) != 0) {
            std::cerr << "    ERROR: EquipmentAggregator build failed!" << std::endl;
            return false;
        }
        
        std::cout << "  → Analyzing accumulated TestSubject statistics from Equipment objects..." << std::endl;
        
        // Execute EquipmentAggregator to get real accumulated TestSubject statistics
        std::string execCmd = "cd " + Paths::ANALYSIS_DIR + " && .\\EquipmentAggregator.exe";
        if (std::system(execCmd.c_str()) != 0) {
            std::cerr << "    ERROR: TestSubject analysis execution failed!" << std::endl;
            return false;
        }
        
        std::cout << "  → TestSubject Equipment analysis complete - data from actual Weapon/Armor objects" << std::endl;
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
