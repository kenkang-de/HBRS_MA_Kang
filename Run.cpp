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
#include "Element/CounterTypeInitializer.h"
#include "Element/SynergyComponentInitializer.h"

#include "Sampling/BatchCreator.h"

#include "Simulator/Simulator.h"

#include "Analysis/TestSubjectToCSV.h"
#include "Analysis/RMSE.h"
#include "Analysis/BalancingLogToCSV.h"

#include "AutoBalancing/Chromosome.h"
#include "AutoBalancing/GeneticBalancingProcessor.h"

class NewMasterController {
private:
    std::string configFile;
    int numBatches;
    int teamsPerBatch;

    ElementList elementList;
    std::unordered_map<std::string, BattleAction> actionMap;
    std::array<Unit, 10> battleUnits;

    //Ratio of strategies
    //BS: Basic, CS: Counter, SYS: Synergistic
    float StrategyRatio_BS;
    float StrategyRatio_CS;
    float StrategyRatio_SYS;
    
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

        } else if (line.find("TEAMS_PER_BATCH=") == 0) {
            teamsPerBatch = std::stoi(line.substr(16));
            
        } else if (line.find("RATIO_BS=") == 0) {
            StrategyRatio_BS = std::stof(line.substr(9));
            
        } else if (line.find("RATIO_CS=") == 0) {
            StrategyRatio_CS = std::stof(line.substr(9));
            
        } else if (line.find("RATIO_SYS=") == 0) {
            StrategyRatio_SYS = std::stof(line.substr(10));
        }
    }
    }
    
    void executeFullPipeline() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Stage 1: Action Loading & Instantiation, Element(Equipment) Loading & Instantiation, Unit Instantiation
        actionMap = LoadActionsFromYAML("Simulator/" + Paths::BATTLE_ACTIONS_YAML);
        EquipmentLoader loader;

        elementList = loader.InstantiateElements(actionMap);
        
        //Set armor, weapons components CounterType
        CounterTypeInitializer counterTypeInitializer(StrategyRatio_CS, &elementList.armors, &elementList.weapons);
        counterTypeInitializer.Init_ArmorList();
        counterTypeInitializer.Init_WeaponList();

        //Set armor, weapon components UnitSynergy(Synergy consists of a weapon and an armor)
        SynergyComponentInitializer synergyComponentInitializer(StrategyRatio_SYS, &elementList.armors, &elementList.weapons);
        synergyComponentInitializer.Init();


        battleUnits = GenerateUnits();
        std::cout<<"Stage 1 Complete"<<std::endl;

        // Stage 2: Sampling & Batch Creation
        BatchCreator batchCreator;
        BatchConfig batchConfig = batchCreator.CreateBatchConfig(numBatches, teamsPerBatch, &elementList.weapons, &elementList.armors);
        std::vector<Batch> batches = batchCreator.CreateBatches(batchConfig);
        std::cout<<"Stage 2 Complete"<<std::endl;

        // Stage 3: Batch Simulation Execution
        Simulator simulator(&elementList, &actionMap, &battleUnits);
        simulator.SimulateBatches(&batches);
        std::cout<<"Stage 3 Complete"<<std::endl;

        // Init_TestSubject
        std::vector<TestSubject*> testSubjects;
        for (Weapon& weapon : elementList.weapons) {
            testSubjects.push_back(&weapon);
        }
        for (Armor& armor : elementList.armors) {
            testSubjects.push_back(&armor);
        }

        //Stage 4: Autobalancing
        GeneticBalancingProcessor balancer;
        balancer.GenerateFirstChromosome(&elementList,testSubjects);
        BalancingLog::InitializeLogs(MAXGENERATION);
        balancer.RunAutoBalancing(&simulator, &batches);
        
        std::cout<<"Stage 4 Complete"<<std::endl;

        // Stage 5: Analysis
        TestSubjectToCSV csvGenerator;
        csvGenerator.Convert(testSubjects);

        BalancingLogToCSV::Convert();

        std::cout<<"Stage 5 Complete"<<std::endl;

auto endTime = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
std::cout << "Total process time: " << duration.count() << " ms" << std::endl;
    }  
};  


int main() {
    std::string configFile = Paths::MASTER_CONFIG; 

    NewMasterController controller(configFile);
    
    controller.LoadConfiguration();
    
    controller.executeFullPipeline();

}
