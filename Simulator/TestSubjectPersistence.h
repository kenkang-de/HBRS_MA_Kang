#ifndef TESTSUBJECT_PERSISTENCE_H
#define TESTSUBJECT_PERSISTENCE_H

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include "SharedEquipmentManager.h"

class TestSubjectPersistence {
public:
    static bool saveTestSubjectData(const std::string& filename = "../Log/V1/TestSubjectData.csv") {
        SharedEquipmentManager& manager = SharedEquipmentManager::getInstance();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[TestSubjectPersistence] Error: Could not create file " << filename << std::endl;
            return false;
        }
        
        // Write header
        file << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
        
        // Save weapon data
        const std::vector<Weapon>& weapons = manager.getWeapons();
        for (const Weapon& weapon : weapons) {
            file << "Weapon," << weapon.GetID() << "," 
                 << weapon.WinRate << "," << weapon.TotalWin << "," 
                 << weapon.TotalLost << "," << weapon.TotalDraw << "," 
                 << weapon.UsageCount << std::endl;
        }
        
        // Save armor data
        const std::vector<Armor>& armor = manager.getArmor();
        for (const Armor& armorPiece : armor) {
            file << "Armor," << armorPiece.GetID() << "," 
                 << armorPiece.WinRate << "," << armorPiece.TotalWin << "," 
                 << armorPiece.TotalLost << "," << armorPiece.TotalDraw << "," 
                 << armorPiece.UsageCount << std::endl;
        }
        
        file.close();
        std::cout << "[TestSubjectPersistence] TestSubject data saved to " << filename << std::endl;
        return true;
    }
    
    static bool loadTestSubjectData(const std::string& filename = "../Log/V1/TestSubjectData.csv") {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[TestSubjectPersistence] No existing TestSubject data found, starting fresh" << std::endl;
            return true; // Not an error - just starting fresh
        }
        
        SharedEquipmentManager& manager = SharedEquipmentManager::getInstance();
        std::string line;
        
        // Skip header
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string type, id, winRateStr, totalWinStr, totalLostStr, totalDrawStr, usageCountStr;
            
            std::getline(ss, type, ',');
            std::getline(ss, id, ',');
            std::getline(ss, winRateStr, ',');
            std::getline(ss, totalWinStr, ',');
            std::getline(ss, totalLostStr, ',');
            std::getline(ss, totalDrawStr, ',');
            std::getline(ss, usageCountStr, ',');
            
            float winRate = std::stof(winRateStr);
            int totalWin = std::stoi(totalWinStr);
            int totalLost = std::stoi(totalLostStr);
            int totalDraw = std::stoi(totalDrawStr);
            int usageCount = std::stoi(usageCountStr);
            
            if (type == "Weapon") {
                Weapon* weapon = manager.getMutableWeaponById(id);
                if (weapon) {
                    weapon->WinRate = winRate;
                    weapon->TotalWin = totalWin;
                    weapon->TotalLost = totalLost;
                    weapon->TotalDraw = totalDraw;
                    weapon->UsageCount = usageCount;
                }
            } else if (type == "Armor") {
                Armor* armor = manager.getMutableArmorById(id);
                if (armor) {
                    armor->WinRate = winRate;
                    armor->TotalWin = totalWin;
                    armor->TotalLost = totalLost;
                    armor->TotalDraw = totalDraw;
                    armor->UsageCount = usageCount;
                }
            }
        }
        
        file.close();
        std::cout << "[TestSubjectPersistence] TestSubject data loaded from " << filename << std::endl;
        return true;
    }
};

#endif
