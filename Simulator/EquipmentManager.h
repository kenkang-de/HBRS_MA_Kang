#ifndef EQUIPMAN_H
#define EQUIPMAN_H

#include <list>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "Armor.h"
#include "Weapon.h"
#include "Unit.h"  


extern std::vector<Armor> ArmorList;   
extern std::vector<Weapon> WeaponList;

void LoadArmorListFromCSV(const std::string& path);

void LoadWeaponListFromCSV(const std::string& path);

Armor GetRandomArmor();

Weapon GetRandomWeapon();

void EquipUnitsRandomEquipments(std::list<Unit>& unitList);

void LoadWeaponListFromCSV(const std::string& filepath, const std::unordered_map<std::string, BattleAction>& actionMap);


#endif