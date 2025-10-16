#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "CounterTypeInitializer.h"
#include "../Simulator/Armor.h"    // Add this include
#include "../Simulator/Weapon.h"   // Add this include

CounterTypeInitializer* CounterTypeInitializer::instance = nullptr;

void CounterTypeInitializer::ResetUsageCount(){
    armorUsageCount[CounterType::Rock] = 0;
    armorUsageCount[CounterType::Scissor] = 0;
    armorUsageCount[CounterType::Paper] = 0;

    weaponUsageCount[CounterType::Rock] = 0;
    weaponUsageCount[CounterType::Scissor] = 0;
    weaponUsageCount[CounterType::Paper] = 0;
};

void CounterTypeInitializer::Init_MaxTypeNumber()
{
    //Weapon and Amor size should be same.
    if(weaponList->size() == armorList->size())
    CS_ElementAmount = armorList->size() * strategyRatio_CS ;

    else
std::cerr<< "[WARNING] weapon and armor size does not match"<< std::endl;

    float calc = CS_ElementAmount / CounterStrategyRelation;
    maxTypeNumber = (calc > 0) ? std::ceil(calc) : 0;
}

void CounterTypeInitializer::Init_ArmorList()
{
    std::vector<int> selectedIndices(CS_ElementAmount);
    
    std::vector<int> allIndices(armorList->size());
    std::iota(allIndices.begin(), allIndices.end(), 0); 
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::sample(allIndices.begin(), allIndices.end(), 
                selectedIndices.begin(), CS_ElementAmount, gen);
    
    for (int randomIndex : selectedIndices) {
        Armor& armor = armorList->at(randomIndex);
        CounterType counterType = GetValidCounterType(armorUsageCount);  
        armor.SetArmorType(counterType);
        armorUsageCount[counterType]++;
    }

//PrintArmorUsageCounts();
}

void CounterTypeInitializer::Init_WeaponList()
{
    std::vector<int> validIndices;
    
    for (size_t i = 0; i < weaponList->size(); i++) {
        Weapon& weapon = weaponList->at(i);
        
        //exclude Enemy targetted action Weapon, to make sure counter strategy is applied correctly.
        if (weapon.GetAction().GetTargetType() == TargetType::ENEMY) {   
            validIndices.push_back(i);
        }
    }
    
    int weaponsToProcess = std::min(CS_ElementAmount, (int)validIndices.size());
    std::vector<int> selectedIndices(weaponsToProcess);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::sample(validIndices.begin(), validIndices.end(), 
                selectedIndices.begin(), weaponsToProcess, gen);
    
    for (int randomIndex : selectedIndices) {
        Weapon& weapon = weaponList->at(randomIndex);
        CounterType counterType = GetValidCounterType(weaponUsageCount); 
        weapon.SetWeaponType(counterType);
        weaponUsageCount[counterType]++;  
    }

    //PrintWeaponUsageCounts();
}

void CounterTypeInitializer::PrintArmorUsageCounts()
{
    std::cout << "=== Armor Usage Counts ===" << std::endl;
    std::cout << "Rock: " << armorUsageCount[CounterType::Rock] << std::endl;
    std::cout << "Paper: " << armorUsageCount[CounterType::Paper] << std::endl;
    std::cout << "Scissor: " << armorUsageCount[CounterType::Scissor] << std::endl;
}

void CounterTypeInitializer::PrintWeaponUsageCounts()
{
    std::cout << "=== Weapon Usage Counts ===" << std::endl;
    std::cout << "Rock: " << weaponUsageCount[CounterType::Rock] << std::endl;
    std::cout << "Paper: " << weaponUsageCount[CounterType::Paper] << std::endl;
    std::cout << "Scissor: " << weaponUsageCount[CounterType::Scissor] << std::endl;
}


CounterType CounterTypeInitializer::GetValidCounterType(std::map<CounterType,int> usageCount)
{
if(maxTypeNumber > 0)
{
  return GetLowestUsageCounterType(usageCount);
} 
else 
    return CounterType::None;
}

CounterType CounterTypeInitializer::GetLowestUsageCounterType(std::map<CounterType,int> usageCount)
{
    if (usageCount.empty()) return CounterType::None;
    
    CounterType lowestType = usageCount.begin()->first;
    int lowestCount = usageCount.begin()->second;
    
    for (const auto& pair : usageCount) {
        if (pair.second < lowestCount) {
            lowestCount = pair.second;
            lowestType = pair.first;
        }
    }
    
    return lowestType;
}