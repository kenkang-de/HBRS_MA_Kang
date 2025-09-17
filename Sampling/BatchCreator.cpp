#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

#include "Batch.h"
#include "BatchCreator.h"
#include "../Simulator/TestSubject.h"
#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"

//How many times does it randomly selects equipment when sampling. 
//When it hits the limit, then the equipment with the lowest usage and  order 
const int RerollLimit =3;

BatchConfig BatchCreator::CreateBatchConfig(int numBatches, int teamsPerBatch, std::vector<Weapon> *weaponList, std::vector<Armor> *armorList) {
    
    BatchConfig config;
    config.numBatches = numBatches;
    config.teamsPerBatch = teamsPerBatch;
    
    for (const Weapon& weapon : *weaponList) {
        EquipmentConfig weaponConfig;
        weaponConfig.id = weapon.GetID();
        config.weaponConfigs.push_back(weaponConfig);
    }
    for (const Armor& armor : *armorList) {
        EquipmentConfig armorConfig;
        armorConfig.id = armor.GetID(); 
        config.armorConfigs.push_back(armorConfig);  
    }   
    return config;
}

// Replace the function with this modern version
int GetRandomEquipmentIndex(int equipmentCount)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, equipmentCount - 1);
    return dis(gen);
}

std::vector<Batch> BatchCreator::CreateBatches(BatchConfig batchConfig) {

std::vector<Batch> batches(batchConfig.numBatches);

int unitCount = batchConfig.numBatches * batchConfig.teamsPerBatch * 5; 
int weaponCount = batchConfig.weaponConfigs.size();
int armorCount = batchConfig.armorConfigs.size();

if(weaponCount != armorCount){
    std::cerr << "ERROR: Weapon and Armor count mismatch!" << std::endl;
    return {};
}

int equipmentCount = weaponCount;

int baseUsage = unitCount / equipmentCount;
int remainder = unitCount % equipmentCount;
int minSampleUsage = baseUsage;
int maxSampleUsage = baseUsage + (remainder > 0 ? 1 : 0);

//Initialize Batch ID and team vectors.
for(int batchIndex = 0; batchIndex < batchConfig.numBatches; batchIndex++) {

    batches[batchIndex].batchId = "Batch_" + std::to_string(batchIndex);
    batches[batchIndex].teams = std::vector<SimulationTeamSetting>(batchConfig.teamsPerBatch);
}

// Distribute Weapon and Armor to units
for(int totalUnitIndex = 0; totalUnitIndex < unitCount; totalUnitIndex++)
 {
    
    SimulationUnitSetting unitSetting;
    int ranCount=0;


    //Weapon
    int ranEquipmentIndex = GetRandomEquipmentIndex(equipmentCount);
    while(ranCount < RerollLimit && batchConfig.weaponConfigs[ranEquipmentIndex].SampleUsageCount >= maxSampleUsage){
    ranEquipmentIndex= GetRandomEquipmentIndex(equipmentCount);
    ranCount++;
    }

    int finalWeaponIndex = ranCount >= RerollLimit ? batchConfig.GetLowestUsageWeaponIndex() : ranEquipmentIndex;

    ranCount=0;

    //Armor
    ranEquipmentIndex = GetRandomEquipmentIndex(equipmentCount);
    while(ranCount < RerollLimit && batchConfig.armorConfigs[ranEquipmentIndex].SampleUsageCount >= maxSampleUsage){
    ranEquipmentIndex= GetRandomEquipmentIndex(equipmentCount);
    ranCount++;
    }

    int finalArmorIndex = ranCount >= RerollLimit ? batchConfig.GetLowestUsageArmorIndex() : ranEquipmentIndex;

    SimulationUnitSetting newUnitSetting;
    newUnitSetting.weaponID = batchConfig.weaponConfigs[finalWeaponIndex].id;
    newUnitSetting.armorID = batchConfig.armorConfigs[finalArmorIndex].id;

    int unitsPerBatch = batchConfig.teamsPerBatch * 5;  
    int batchIndex = totalUnitIndex / unitsPerBatch;     
    int teamIndex = (totalUnitIndex % unitsPerBatch) / 5; 
    int unitIndex = totalUnitIndex % 5;          

    batches[batchIndex].teams[teamIndex].simulationUnitSettings[unitIndex] = newUnitSetting;

    batchConfig.weaponConfigs[finalWeaponIndex].SampleUsageCount++;
    batchConfig.armorConfigs[finalArmorIndex].SampleUsageCount++;
}

return batches;
}



int BatchConfig::GetLowestUsageArmorIndex()
{
     int lowestUsageIndex = 0;  
    
    for(int i = 0; i < armorConfigs.size(); i++)  
    {
        if (armorConfigs[i].SampleUsageCount < armorConfigs[lowestUsageIndex].SampleUsageCount) {
            lowestUsageIndex = i;  
        }
        else if (armorConfigs[i].SampleUsageCount == armorConfigs[lowestUsageIndex].SampleUsageCount) {
        }
    }
    return lowestUsageIndex;  
}

int BatchConfig::GetLowestUsageWeaponIndex()
{
        int lowestUsageIndex = 0;  
    
    for(int i = 0; i < weaponConfigs.size(); i++)  
    {
        if (weaponConfigs[i].SampleUsageCount < weaponConfigs[lowestUsageIndex].SampleUsageCount) {
            lowestUsageIndex = i;  
        }
        else if (weaponConfigs[i].SampleUsageCount == weaponConfigs[lowestUsageIndex].SampleUsageCount) {
        }
    }
    return lowestUsageIndex;  
}






