#include <cmath>
#include <random>

#include "ArmorTypeInitializer.h"

std::map<ArmorType, int> ArmorTypeInitializer::usageCount;
ArmorTypeInitializer* ArmorTypeInitializer::instance = nullptr;

void ArmorTypeInitializer::ResetUsageCount(){
    usageCount[ArmorType::Light] = 0;
    usageCount[ArmorType::Medium] = 0;
    usageCount[ArmorType::Heavy] = 0;
};

void ArmorTypeInitializer::Init_MaxTypeNumber()
{
    totalElementNumber_CS = armorList->size() * strategyRatio_CS * 2;
    float calc = totalElementNumber_CS / CounterStrategyRelation;
    maxTypeNumber = (calc > 0) ? std::ceil(calc) : 0;
}

void ArmorTypeInitializer::Init_ArmorList()
{
   for (size_t i = 0; i < totalElementNumber_CS ;i++) {
        Armor& armor = armorList->at(i);
        ArmorType armorType = GetValidArmorType();
        armor.SetArmorType(armorType);
        usageCount[armorType]++;
    }
}

ArmorType ArmorTypeInitializer::GetValidArmorType(){
if(maxTypeNumber > 0)
{
  if(AreAllUsageCountsSame())
  return GetRandomArmorType();
  else
  return GetLowestUsageArmorType();
} 
  
else 
    return ArmorType::None;
}

ArmorType ArmorTypeInitializer::GetRandomArmorType()
{
  ArmorType validTypes[] = {ArmorType::Light, ArmorType::Medium, ArmorType::Heavy};
        int numValidTypes = 3;
        
        // Generate random index
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, numValidTypes - 1);

        return validTypes[dis(gen)];
}

bool ArmorTypeInitializer::AreAllUsageCountsSame()
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

ArmorType ArmorTypeInitializer::GetLowestUsageArmorType()
{
    if (usageCount.empty()) return ArmorType::None;
    
    ArmorType lowestType = usageCount.begin()->first;
    int lowestCount = usageCount.begin()->second;
    
    for (const auto& pair : usageCount) {
        if (pair.second < lowestCount) {
            lowestCount = pair.second;
            lowestType = pair.first;
        }
    }
    
    return lowestType;
}