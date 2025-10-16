#include "SynergyComponentInitializer.h"

SynergyComponentInitializer* SynergyComponentInitializer::instance = nullptr;

void SynergyComponentInitializer::Init_SYS_ElementAmount()
{
    //Weapon and Amor size should be same.
    if(weaponList->size() == armorList->size())
    SYS_ElementAmount = armorList->size() * 2 * strategyRatio_SYS ;

    else
std::cerr<< "[WARNING] weapon and armor size does not match"<< std::endl;

    SYS_ElementAmount = (SYS_ElementAmount > 0) ? std::ceil(SYS_ElementAmount) : 0;

        std::cout<<"Total Synergy Component: " << SYS_ElementAmount << std::endl;
}
    std::vector<Armor*> SynergyComponentInitializer::GetRandomArmorPointers(int amount)
{
    std::vector<Armor*> armorPointers;
    armorPointers.reserve(armorList->size());
    
    for (auto& armor : *armorList) {
        armorPointers.push_back(&armor);
    }
    
    std::vector<Armor*> randomArmorPointers;
    randomArmorPointers.reserve(amount);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::sample(armorPointers.begin(), armorPointers.end(), 
                std::back_inserter(randomArmorPointers), amount, gen);
    
    return randomArmorPointers;
}

std::vector<Weapon*> SynergyComponentInitializer::GetRandomWeaponPointers(int amount)
{
    std::vector<Weapon*> weaponPointers;
    weaponPointers.reserve(weaponList->size());
    
    for (auto& weapon : *weaponList) {
        weaponPointers.push_back(&weapon);
    }
    
    std::vector<Weapon*> randomWeaponPointers;
    randomWeaponPointers.reserve(amount);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::sample(weaponPointers.begin(), weaponPointers.end(), 
                std::back_inserter(randomWeaponPointers), amount, gen);
    
    return randomWeaponPointers;
}

void SynergyComponentInitializer::Generate_UnitSynergyList(std::vector<Armor*> armorPtrs, std::vector<Weapon*> weaponPtrs)
{
    size_t minSize = std::min(armorPtrs.size(), weaponPtrs.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        std::string key = "Synergy_" + std::to_string(i);
        
        armorPtrs[i]->GetUnitSynergyIDs().push_back(key);
        weaponPtrs[i]->GetUnitSynergyIDs().push_back(key);
        
        UnitSynergy unitSynergy;
        unitSynergy.armorptr = armorPtrs[i];
        unitSynergy.weaponptr = weaponPtrs[i];
        // Stat(int attack, int defense, int hitpoint, int speed, int threat)
        unitSynergy.effectStat = Stat(0, 0, 0, 3, 0);
        
        SynergyRule::unitSynergyMap[key] = unitSynergy;
    }
}

void SynergyComponentInitializer::Init()
{
    SYS_ElementAmount = (SYS_ElementAmount > 0) ? std::ceil(SYS_ElementAmount) : 0;

    int amountA = (SYS_ElementAmount+1)/2;
    int amountW = SYS_ElementAmount/2;

    Generate_UnitSynergyList(GetRandomArmorPointers(amountA), GetRandomWeaponPointers(amountW));
}



