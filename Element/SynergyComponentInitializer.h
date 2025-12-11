#ifndef SYNERGY_INIT_H
#define SYNERGY_INIT_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

#include "../Simulator/Armor.h"
#include "../Simulator/SynergyRule.h"
#include "../Simulator/Weapon.h"

class SynergyComponentInitializer {
  private:
    float strategyRatio_SYS;

    int SYS_ElementAmount;

    static SynergyComponentInitializer *instance;

    std::vector<Armor> *armorList;

    std::vector<Weapon> *weaponList;

    std::vector<Armor *> GetRandomArmorPointers(int amount);

    std::vector<Weapon *> GetRandomWeaponPointers(int amount);

    void Generate_UnitSynergyList(std::vector<Armor *> armorPtrs, std::vector<Weapon *> weaponPtrs);

    void Init_SYS_ElementAmount(float strategyRatio_SYS);

    void Reset();

  public:
    static SynergyComponentInitializer *GetInstance() {
        return instance;
    }

    SynergyComponentInitializer(std::vector<Armor> *_armorList, std::vector<Weapon> *_weaponList)
        : armorList(_armorList), weaponList(_weaponList) {
        instance = this;
    };

    void Init(float strategyRatio_SYS);
};

#endif