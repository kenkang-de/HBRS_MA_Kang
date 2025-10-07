#ifndef ARMORTYPE_INIT_H
#define ARMORTYPE_INIT_H

#include <map>
#include <vector>

#include "../Simulator/Armor.h"

const int CounterStrategyRelation = 3;

//Armor Initializer has to be instantiated before the ElementInstantiation
class ArmorTypeInitializer
{
    private:
    float strategyRatio_CS;

    static ArmorTypeInitializer* instance;

    std::vector<Armor>* armorList;

    int maxTypeNumber;

    int totalElementNumber_CS;

    public:
    static std::map<ArmorType,int> usageCount;

    static ArmorTypeInitializer* GetInstance() { return instance; }

    ArmorTypeInitializer(float _strategyRatio_CS, std::vector<Armor>* _armorList) : 
    strategyRatio_CS(_strategyRatio_CS), armorList(_armorList) 
    {
    instance = this;
    ResetUsageCount();
    Init_MaxTypeNumber();
    };

    void ResetUsageCount();

    void Init_MaxTypeNumber();

    void Init_ArmorList();

    ArmorType GetValidArmorType();

    ArmorType GetLowestUsageArmorType();

    ArmorType GetRandomArmorType();

    bool AreAllUsageCountsSame();

};


#endif