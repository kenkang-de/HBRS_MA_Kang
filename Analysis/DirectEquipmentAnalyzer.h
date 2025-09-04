#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iomanip>

// Forward declarations
#include "Weapon.h"
#include "Armor.h"

class DirectEquipmentAnalyzer {
private:
    std::vector<const Weapon*> weapons;
    std::vector<const Armor*> armors;
    
public:
    // Constructor that takes equipment containers
    DirectEquipmentAnalyzer(const std::vector<const Weapon*>& weaponContainer, 
                           const std::vector<const Armor*>& armorContainer)
        : weapons(weaponContainer), armors(armorContainer) {}
    
    void generateAnalysisReport(const std::string& outputPath = "../Log/V1/DirectEquipmentAnalysis.csv") {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            std::cerr << "[DirectAnalyzer] Error: Could not create output file: " << outputPath << std::endl;
            return;
        }
        
        std::cout << "[DirectAnalyzer] Analyzing equipment statistics directly from objects..." << std::endl;
        
        // Write CSV header
        file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
        
        // Analyze weapons
        std::cout << "[DirectAnalyzer] Processing " << weapons.size() << " weapons..." << std::endl;
        for (const Weapon* weapon : weapons) {
            if (!weapon) continue;
            
            file << "Weapon," << weapon->GetID() << "," 
                 << std::fixed << std::setprecision(1) << (weapon->WinRate * 100.0f) << ","
                 << weapon->TotalWin << "," << weapon->TotalLost << "," 
                 << weapon->TotalDraw << "," << weapon->UsageCount << std::endl;
        }
        
        // Analyze armor
        std::cout << "[DirectAnalyzer] Processing " << armors.size() << " armor pieces..." << std::endl;
        for (const Armor* armor : armors) {
            if (!armor) continue;
            
            file << "Armor," << armor->GetID() << "," 
                 << std::fixed << std::setprecision(1) << (armor->WinRate * 100.0f) << ","
                 << armor->TotalWin << "," << armor->TotalLost << "," 
                 << armor->TotalDraw << "," << armor->UsageCount << std::endl;
        }
        
        file.close();
        
        printSummary();
        
        std::cout << "[DirectAnalyzer] Direct equipment analysis saved to: " << outputPath << std::endl;
    }
    
    void printSummary() {
        int activeWeapons = 0, activeArmor = 0;
        int totalWeaponUsage = 0, totalArmorUsage = 0;
        
        // Count active equipment (with usage > 0)
        for (const Weapon* weapon : weapons) {
            if (weapon && weapon->UsageCount > 0) {
                activeWeapons++;
                totalWeaponUsage += weapon->UsageCount;
            }
        }
        
        for (const Armor* armor : armors) {
            if (armor && armor->UsageCount > 0) {
                activeArmor++;
                totalArmorUsage += armor->UsageCount;
            }
        }
        
        std::cout << "\n=== Direct Equipment Analysis Summary ===" << std::endl;
        std::cout << "Total Weapons Available: " << weapons.size() << std::endl;
        std::cout << "Active Weapons (Used): " << activeWeapons << std::endl;
        std::cout << "Total Weapon Usage: " << totalWeaponUsage << std::endl;
        std::cout << "Total Armor Available: " << armors.size() << std::endl;
        std::cout << "Active Armor (Used): " << activeArmor << std::endl;
        std::cout << "Total Armor Usage: " << totalArmorUsage << std::endl;
        
        // Find best performers
        const Weapon* bestWeapon = nullptr;
        const Armor* bestArmor = nullptr;
        float maxWeaponWR = -1.0f, maxArmorWR = -1.0f;
        
        for (const Weapon* weapon : weapons) {
            if (weapon && weapon->UsageCount > 0 && weapon->WinRate > maxWeaponWR) {
                maxWeaponWR = weapon->WinRate;
                bestWeapon = weapon;
            }
        }
        
        for (const Armor* armor : armors) {
            if (armor && armor->UsageCount > 0 && armor->WinRate > maxArmorWR) {
                maxArmorWR = armor->WinRate;
                bestArmor = armor;
            }
        }
        
        if (bestWeapon) {
            std::cout << "Best Weapon: " << bestWeapon->GetID() 
                      << " (" << std::fixed << std::setprecision(1) << (bestWeapon->WinRate * 100.0f) << "% win rate)" << std::endl;
        }
        if (bestArmor) {
            std::cout << "Best Armor: " << bestArmor->GetID() 
                      << " (" << std::fixed << std::setprecision(1) << (bestArmor->WinRate * 100.0f) << "% win rate)" << std::endl;
        }
    }
};
