#ifndef COUNTERTYPE_INIT_H
#define COUNTERTYPE_INIT_H

#include <map>
#include <vector>

const int CounterStrategyRelation = 3;

enum CounterType { None, Rock, Scissor, Paper };

class Armor;
class Weapon;

// Initialize CounterType to GameElements(RPS)
// Initialize order: 1.Armor -> 2.Weapon
class CounterTypeInitializer {
  private:
    float strategyRatio_CS;

    static CounterTypeInitializer *instance;

    std::vector<Armor> *armorList;

    std::vector<Weapon> *weaponList;

    int maxTypeNumber;

    int CS_ElementAmount;

    std::map<CounterType, int> armorUsageCount;

    std::map<CounterType, int> weaponUsageCount;

  public:
    static CounterTypeInitializer *GetInstance() {
        return instance;
    }

    CounterTypeInitializer(std::vector<Armor> *_armorList, std::vector<Weapon> *_weaponList)
        : armorList(_armorList), weaponList(_weaponList) {
        instance = this;
    };

    void ResetUsageCount();

    void Init_MaxTypeNumber();

    void Init(float strategyRatio_CS);

    void Reset_ArmorList();
    void Init_ArmorList();

    void Reset_WeaponList();
    void Init_WeaponList();

    CounterType GetValidCounterType(std::map<CounterType, int> usageCount);

    CounterType GetLowestUsageCounterType(std::map<CounterType, int> usageCount);

    void PrintArmorUsageCounts();

    void PrintWeaponUsageCounts();
};

#endif