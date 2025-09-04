#include <iostream>
#include <vector>
#include "SharedEquipmentManager.h"
#include "../Analysis/DirectEquipmentAnalyzer.h"

int main() {
    std::cout << "=== Direct Equipment Analysis Tool ===" << std::endl;
    
    // Load equipment data
    SharedEquipmentManager& equipmentManager = SharedEquipmentManager::getInstance();
    equipmentManager.loadEquipment();
    
    // Get equipment containers
    const std::vector<Weapon>& weapons = equipmentManager.getWeapons();
    const std::vector<Armor>& armor = equipmentManager.getArmor();
    
    // Convert to pointer vectors for the analyzer
    std::vector<const Weapon*> weaponPtrs;
    std::vector<const Armor*> armorPtrs;
    
    for (const auto& weapon : weapons) {
        weaponPtrs.push_back(&weapon);
    }
    
    for (const auto& armorPiece : armor) {
        armorPtrs.push_back(&armorPiece);
    }
    
    // Create analyzer and generate report
    DirectEquipmentAnalyzer analyzer(weaponPtrs, armorPtrs);
    analyzer.generateAnalysisReport();
    
    std::cout << "\n=== Direct Analysis Complete ===" << std::endl;
    return 0;
}
