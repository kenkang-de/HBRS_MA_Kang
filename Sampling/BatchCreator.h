#ifndef BATCHCREATOR_H
#define BATCHCREATOR_H

#include <vector>
#include <string>
#include "Batch.h"
#include "../Simulator/Weapon.h"
#include "../Simulator/Armor.h"

class EquipmentConfig
{
    public:
std::string id;
int SampleUsageCount = 0;
};

class BatchConfig
{
    public:

    int numBatches;
    int teamsPerBatch;

    std::vector<EquipmentConfig> weaponConfigs;
    std::vector<EquipmentConfig> armorConfigs;

    int GetLowestUsageWeaponIndex();
    int GetLowestUsageArmorIndex();
};

class BatchCreator
{
public:
BatchConfig CreateBatchConfig(int numBatches, int teamsPerBatch, std::vector<Weapon> *weaponList, std::vector<Armor> *armorList);
std::vector<Batch> CreateBatches(BatchConfig config);
};


#endif 