#include <cmath>
#include <random>
#include <iostream>

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
    std::cerr<< "[WARNING} weapon and armor size does not match"<< std::endl;

    float calc = CS_ElementAmount / CounterStrategyRelation;
    maxTypeNumber = (calc > 0) ? std::ceil(calc) : 0;
}

void CounterTypeInitializer::Init_ArmorList()
{
   for (int i = 0; i < CS_ElementAmount ;i++) {
        Armor& armor = armorList->at(i);
        CounterType CounterType = GetValidCounterType(armorUsageCount);
        armor.SetArmorType(CounterType);
        armorUsageCount[CounterType]++;
    }
}

void CounterTypeInitializer::Init_WeaponList()
{
      for (int i = 0; i < CS_ElementAmount ;i++) {
        Weapon& weapon = weaponList->at(i);
        CounterType CounterType = GetValidCounterType(weaponUsageCount);
        weapon.SetWeaponType(CounterType);
        armorUsageCount[CounterType]++;
    } 
}

CounterType CounterTypeInitializer::GetValidCounterType(std::map<CounterType,int> usageCount){
if(maxTypeNumber > 0)
{
  if(AreAllUsageCountsSame(usageCount))
  return GetRandomCounterType();
  else
  return GetLowestUsageArmorType();
} 
  
else 
    return CounterType::None;
}

CounterType CounterTypeInitializer::GetRandomCounterType()
{
  CounterType validTypes[] = {CounterType::Rock, CounterType::Paper, CounterType::Scissor};
        int numValidTypes = 3;
        
        // Generate random index
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, numValidTypes - 1);

        return validTypes[dis(gen)];
}

bool CounterTypeInitializer::AreAllUsageCountsSame(std::map<CounterType,int> usageCount)
{
    if (usageCount.empty()) return true;
    
    int referenceCount = usageCount.begin()->second;
    
    for (const auto& pair : usageCount) {
        if (pair.second != referenceCount) {
            return false;
        }
    }
    
    return true;
}

CounterType CounterTypeInitializer::GetLowestUsageArmorType()
{
    if (armorUsageCount.empty()) return CounterType::None;
    
    CounterType lowestType = armorUsageCount.begin()->first;
    int lowestCount = armorUsageCount.begin()->second;
    
    for (const auto& pair : armorUsageCount) {
        if (pair.second < lowestCount) {
            lowestCount = pair.second;
            lowestType = pair.first;
        }
    }
    
    return lowestType;
}