#include "Paths.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <filesystem>
#include <vector>

#include "Element/ElementList.h"
#include "Element/EquipmentLoader.h"
#include "Element/BattleActionLoader.h"
#include "Element/UnitGenerator.h"

#include "Sampling/BatchCreator.h"

class NewMasterController {
private:
    std::string configFile;
    int numBatches;
    int teamsPerBatch;

    ElementList elementList;
    std::unordered_map<std::string, BattleAction> actionMap;
    
public:
    NewMasterController(const std::string& config) : configFile(config) {}
    
    // Load configuration from config file
    void LoadConfiguration() {
        std::ifstream file(configFile);
        
        std::string line;

        while (std::getline(file, line)) {

            if (line.empty() || line[0] == '#') continue;

            if (line.find("NUM_BATCHES=") == 0) {
            numBatches = std::stoi(line.substr(12));

            }else if (line.find("TEAMS_PER_BATCH=") == 0) {
                teamsPerBatch = std::stoi(line.substr(16));
            }
        }
    }
    
    void executeFullPipeline() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Stage 1: Action Loading & Instantiation, Element(Equipment) Loading & Instantiation, Unit Instantiation
        actionMap = LoadActionsFromYAML("Simulator/" + Paths::BATTLE_ACTIONS_YAML);
        executeElementGeneration();
        std::array<Unit, 10> AllUnits = GenerateUnits();
        std::cout<<"Stage 1 Complete"<<std::endl;

        // Stage 2: Sampling & Batch Creation
        BatchCreator batchCreator;
        BatchConfig batchConfig = batchCreator.CreateBatchConfig(numBatches, teamsPerBatch, &elementList.weapons, &elementList.armors);
        std::vector<Batch> batches = batchCreator.CreateBatches(batchConfig);
        // // Stage 3: Batch Simulation Execution
        // if (!executeBatchSimulations()) {
        //     std::cerr << "ERROR: Batch simulation execution failed!" << std::endl;
        //     return 3;
        // }
        
        // // Calculate number of batches (assuming configs are divided into batches)
        // int numBatches = (numBatches + 9) / 10; // Round up division for batches of 10
        // std::cout << "Batches: " << numBatches << std::endl;
        
        // // Final timing
        // auto endTime = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // std::cout << "Total execution time: ";
        // if (duration.count() >= 1000) {
        //     auto seconds = duration.count() / 1000.0;
        //     std::cout << std::fixed << std::setprecision(2) << seconds << " seconds";
        // } else {
        //     std::cout << duration.count() << " milliseconds";
        // }
        // std::cout << std::endl;
        
        // return 0;
    }
    
private:

    void executeElementGeneration() {
        EquipmentLoader loader;
        elementList = loader.InstantiateElements(actionMap);
    }
    
    bool executeSampling() {
        
        // Execute Batch Creator to create batch configurations
        std::string execCmd = "cd " + Paths::SAMPLING_DIR + " ; .\\BatchCreator.exe" + 
        std::to_string(numBatches) + " " +      // argv[1]
        std::to_string(teamsPerBatch) + " " +   // argv[2] 
        " >nul 2>&1";  

        int execResult = std::system(execCmd.c_str());
        if (execResult != 0) {
            std::cerr << "    ERROR: Batch Creator execution failed with exit code: " << execResult << std::endl;
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
        
        
        // Build Batch Executor (suppress output)
        std::string buildRunnerCmd = "cd " + Paths::SAMPLING_DIR + " && g++ -std=c++17 BatchExecutor.cpp -o BatchExecutor.exe >nul 2>&1";
        if (std::system(buildRunnerCmd.c_str()) != 0) {
            std::cerr << "    ERROR: Batch Executor build failed!" << std::endl;
            return false;
        }
        
        
        // Execute batch simulations (suppress output)
        // std::string execCmd = "cd " + Paths::SAMPLING_DIR + " && .\\BatchExecutor.exe " + std::to_string(simulationsPerBatch) + " >nul 2>&1";
        // if (std::system(execCmd.c_str()) != 0) {
        //     std::cerr << "    ERROR: Batch simulation execution failed!" << std::endl;
        //     return false;
        // }
        
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

int main() {
    std::string configFile = Paths::MASTER_CONFIG; 

    NewMasterController controller(configFile);
    
    controller.LoadConfiguration();
    
    controller.executeFullPipeline();
}
