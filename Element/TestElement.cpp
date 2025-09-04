#include "TeamConfiguration.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== Clean Element System Test ===" << std::endl;
    
    // Get number of configurations from command line argument
    int numConfigs = 5; // default
    if (argc > 1) {
        numConfigs = std::atoi(argv[1]);
        if (numConfigs <= 0) numConfigs = 5;
    }
    
    std::cout << "Generating " << numConfigs << " battle configurations..." << std::endl;
    
    // Initialize the team configuration system
    TeamConfiguration teamConfig;
    
    if (!teamConfig.initialize()) {
        std::cerr << "Failed to initialize team configuration system" << std::endl;
        return 1;
    }
    
    // Generate battle configurations
    std::cout << "\nGenerating battle configurations..." << std::endl;
    teamConfig.generateBattleConfigs(numConfigs);
    
    // Print statistics
    teamConfig.printStats();
    
    // Export configurations
    std::cout << "\nExporting configurations..." << std::endl;
    teamConfig.exportBattleConfigs("element_test_configs.csv");
    
    // Simulate some battle results to test statistics tracking
    std::cout << "\nSimulating battle results..." << std::endl;
    auto& weapons = teamConfig.getMutableWeapons();
    auto& armor = teamConfig.getMutableArmor();
    
    // Simulate random battle outcomes
    for (auto& weapon : weapons) {
        if (weapon.UsageCount > 0) {
            weapon.recordWin();
            weapon.recordLoss();
            weapon.recordDraw();
        }
    }
    
    for (auto& armorItem : armor) {
        if (armorItem.UsageCount > 0) {
            armorItem.recordWin();
            armorItem.recordWin();
            armorItem.recordLoss();
        }
    }
    
    // Export final results (without console output)
    teamConfig.exportFinalAnalysis("element_final_analysis.csv");
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}
