#include "Paths.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "Element/BattleActionLoader.h"
#include "Element/ComponentSelector.h"
#include "Element/CounterTypeInitializer.h"
#include "Element/ElementList.h"
#include "Element/EquipmentLoader.h"
#include "Element/SynergyComponentInitializer.h"
#include "Element/UnitGenerator.h"

#include "Sampling/BatchCreator.h"

#include "Simulator/Simulator.h"

#include "Analysis/BalancingLogToCSV.h"
#include "Analysis/GameCoponentToCSV.h"
#include "Analysis/RMSE.h"
#include "Analysis/TestSubjectToCSV.h"

#include "AutoBalancing/Chromosome.h"
#include "AutoBalancing/GeneticBalancingProcessor.h"

#include "ExperimentSettings.h"

class NewMasterController {

  private:
    std::string configFile;
    int numBatches;
    int teamsPerBatch;
    int totalGameComponentNumber;

    ElementList originalElementList;
    std::unordered_map<std::string, BattleAction> actionMap;
    std::array<Unit, 10> battleUnits;

  public:
    NewMasterController(const std::string &config) : configFile(config) {}

    RUNMODE runMode;

    // Load configuration from config file
    void LoadConfiguration() {
        std::ifstream file(configFile);

        std::string line;

        while (std::getline(file, line)) {

            if (line.empty() || line[0] == '#')
                continue;

            if (line.find("NUM_BATCHES=") == 0) {
                numBatches = std::stoi(line.substr(12));

            } else if (line.find("TEAMS_PER_BATCH=") == 0) {
                teamsPerBatch = std::stoi(line.substr(16));

            } else if (line.find("TOTAL_GAMECOMPONENTNUMBER=") == 0) {
                totalGameComponentNumber = std::stoi(line.substr(26));
            }
        }
    }

    void executeFullPipeline(int executionNumber) {
        auto startTime = std::chrono::high_resolution_clock::now();

        // Set batch configuration in ExperimentSettings
        ExperimentSettings::NUM_BATCHES = numBatches;
        ExperimentSettings::TEAMS_PER_BATCH = teamsPerBatch;

        ExperimentSettings settings;
        std::vector<std::string> settingFiles = settings.GetExperimentFiles("./ExperimentSettings");

        std::string outerFolder = std::to_string(totalGameComponentNumber) + "_" + std::to_string(executionNumber);

        // Load action map
        actionMap = LoadActionsFromYAML("Simulator/" + Paths::BATTLE_ACTIONS_YAML);
        EquipmentLoader loader;

        originalElementList = loader.InstantiateElements(actionMap);

        ElementList elementList;
        ComponentSelector selector;
        elementList = selector.SelectRandomCompoent(originalElementList, totalGameComponentNumber);

        // Create batches
        BatchCreator batchCreator;
        BatchConfig batchConfig =
            batchCreator.CreateBatchConfig(numBatches, teamsPerBatch, &elementList.weapons, &elementList.armors);
        std::vector<Batch> batches = batchCreator.CreateBatches(batchConfig);
        std::cout << "Batch created." << std::endl;

        int experimentCount = 1;
        for (const std::string &fileDir : settingFiles) {

            std::cout << "EXPERIMENT " << experimentCount << "/" << settingFiles.size() << std::endl;
            std::cout << "Total GameComponent: " + totalGameComponentNumber << std::endl;

            settings.LoadFromFile(fileDir);

            std::string innerFolder = settings.GetExperimentFolderName();
            std::string experimentPath = "./Log/Balancing/" + outerFolder + "/" + innerFolder;
            std::filesystem::create_directories(experimentPath);

            std::filesystem::copy_file(fileDir, experimentPath + "/Configuration.txt",
                                       std::filesystem::copy_options::overwrite_existing);

            // Set armor, weapons components CS
            CounterTypeInitializer counterTypeInitializer(&elementList.armors, &elementList.weapons);
            counterTypeInitializer.Init(ExperimentSettings::RATIO_CS);
            counterTypeInitializer.Init_ArmorList();
            counterTypeInitializer.Init_WeaponList();

            // Set armor, weapon components SYS
            SynergyComponentInitializer synergyComponentInitializer(&elementList.armors, &elementList.weapons);
            synergyComponentInitializer.Init(ExperimentSettings::RATIO_SYS);
            SynergyRule::PrintTotalUnitSynergyApplied();

            battleUnits = GenerateUnits();

            Simulator simulator(&elementList, &actionMap, &battleUnits);
            simulator.SimulateBatches(&batches);
            std::cout << "Initial batches simulated." << std::endl;

            // Init_TestSubject
            std::vector<TestSubject *> testSubjects;
            for (Weapon &weapon : elementList.weapons) {
                testSubjects.push_back(&weapon);
            }
            for (Armor &armor : elementList.armors) {
                testSubjects.push_back(&armor);
            }

            // Log first Simulated batch
            TestSubjectToCSV csvGenerator;
            csvGenerator.Convert(testSubjects);

            // Autobalancing
            GeneticBalancingProcessor balancer;
            balancer.GenerateFirstChromosome(&elementList, testSubjects);

            std::cout << "Auto balancing starts" << std::endl;

            BalancingLogToCSV::SetExperimentPath(experimentPath);
            GameComponentToCSV::SetExperimentPath(experimentPath);

            BalancingLog::InitializeLogs(ExperimentSettings::MAXGENERATION);
            balancer.RunAutoBalancing(&simulator, &batchConfig, batches);

            BalancingLogToCSV::Convert();
            GameComponentToCSV::Convert(testSubjects);

            experimentCount++;
        }

        std::cout << "Auto balancing ended" << std::endl;

        // TimeCheck
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Total process time: " << duration.count() << " ms" << std::endl;
    }
};

int main(int argc, char *argv[]) {

    std::string configFile = Paths::MASTER_CONFIG;

    for (int execNum = 1; execNum <= std::atoi(argv[1]); execNum++) {

        NewMasterController controller(configFile);
        controller.LoadConfiguration();
        controller.executeFullPipeline(execNum);
    }

    std::cout << "\nAll executions completed." << std::endl;
    return 0;
}
