#ifndef SYNERGY_INIT_H
#define SYNERGY_INIT_H

#include <vector>
#include <algorithm>
#include <memory>
#include <random>
#include <iterator>
#include <iostream>
#include <unordered_map> 

#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"
#include "../Simulator/SynergyRule.h"

class SynergyComponentInitializer
{
    private:
    float strategyRatio_SYS;

    int SYS_ElementAmount;

    static SynergyComponentInitializer* instance;

    std::vector<Armor>* armorList;

    std::vector<Weapon>* weaponList;

    std::vector<Armor*> GetRandomArmorPointers(int amount);

    std::vector<Weapon*> GetRandomWeaponPointers(int amount);

    void Generate_UnitSynergyList(std::vector<Armor*> armorPtrs, std::vector<Weapon*> weaponPtrs);

    void Init_SYS_ElementAmount();


    public:

    static SynergyComponentInitializer* GetInstance() { return instance; }

    SynergyComponentInitializer(float _strategyRatio_SYS, std::vector<Armor>* _armorList, std::vector<Weapon>* _weaponList) : 
    strategyRatio_SYS(_strategyRatio_SYS), armorList(_armorList), weaponList(_weaponList) 
    {
    instance = this;
    Init_SYS_ElementAmount();
    };

    void Init();

};

#endif