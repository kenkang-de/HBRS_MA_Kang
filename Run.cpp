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

    ElementList elementList;
    std::unordered_map<std::string, BattleAction> actionMap;
    std::array<Unit, 10> battleUnits;

  public:
    NewMasterController(const std::string &config) : configFile(config) {}

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
            }
        }
    }

    void executeFullPipeline() {
        auto startTime = std::chrono::high_resolution_clock::now();

        ExperimentSettings settings;
        std::vector<std::string> settingFiles = settings.GetExperimentFiles("./ExperimentSettings");

        // Stage 1: Action Loading & Instantiation, Element(Equipment) Loading & Instantiation, Unit Instantiation
        actionMap = LoadActionsFromYAML("Simulator/" + Paths::BATTLE_ACTIONS_YAML);
        EquipmentLoader loader;

        elementList = loader.InstantiateElements(actionMap);

        ComponentSelector selector;
        elementList = selector.SelectRandomCompoent(elementList, 40);

        // Set armor, weapons components CounterType
        CounterTypeInitializer counterTypeInitializer(&elementList.armors, &elementList.weapons);
        counterTypeInitializer.Init(ExperimentSettings::RATIO_CS);
        counterTypeInitializer.Init_ArmorList();
        counterTypeInitializer.Init_WeaponList();

        // Set armor, weapon components UnitSynergy(Synergy consists of a weapon and an armor)
        SynergyComponentInitializer synergyComponentInitializer(&elementList.armors, &elementList.weapons);
        synergyComponentInitializer.Init(ExperimentSettings::RATIO_SYS);
        SynergyRule::PrintTotalUnitSynergyApplied();

        battleUnits = GenerateUnits();
        std::cout << "Stage 1 Complete" << std::endl;

        // Stage 2: Sampling & Batch Creation
        BatchCreator batchCreator;
        BatchConfig batchConfig =
            batchCreator.CreateBatchConfig(numBatches, teamsPerBatch, &elementList.weapons, &elementList.armors);
        std::vector<Batch> batches = batchCreator.CreateBatches(batchConfig);
        std::cout << "Stage 2 Complete" << std::endl;

        // Stage 3: Batch Simulation Execution
        Simulator simulator(&elementList, &actionMap, &battleUnits);
        simulator.SimulateBatches(&batches);
        std::cout << "Stage 3 Complete" << std::endl;

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

        // Stage 4: Autobalancing
        GeneticBalancingProcessor balancer;
        balancer.GenerateFirstChromosome(&elementList, testSubjects);

        std::cout << "Auto balancing starts" << std::endl;

        BalancingLogToCSV::InitializeRunDirectory();

        GameComponentToCSV::SetSharedDirectory(BalancingLogToCSV::GetSharedDirectory());

        // Setting files found - run for each file
        int experimentCount = 1;
        for (const std::string &fileDir : settingFiles) {
            std::cout << "EXPERIMENT " << experimentCount << "/" << settingFiles.size() << std::endl;
            std::cout << "Loading settings from: " << fileDir << std::endl;

            BalancingLogToCSV::SetExperimentNumber(experimentCount);
            GameComponentToCSV::SetExperimentNumber(experimentCount);

            settings.LoadFromFile(fileDir);

            // Init CS
            counterTypeInitializer.Init(ExperimentSettings::RATIO_CS);
            counterTypeInitializer.Init_ArmorList();
            counterTypeInitializer.Init_WeaponList();

            // Init SYS
            synergyComponentInitializer.Init(ExperimentSettings::RATIO_SYS);
            SynergyRule::PrintTotalUnitSynergyApplied();

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

int main() {
    std::string configFile = Paths::MASTER_CONFIG;

    NewMasterController controller(configFile);

    controller.LoadConfiguration();

    controller.executeFullPipeline();
}
