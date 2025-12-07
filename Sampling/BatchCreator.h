#ifndef BATCHCREATOR_H
#define BATCHCREATOR_H

#include "../Constants.h"
#include "../Simulator/Armor.h"
#include "../Simulator/Weapon.h"
#include "Batch.h"
#include "NoveltyChecker.h"

#include <string>
#include <vector>

class EquipmentConfig {
  public:
    std::string id;
    int SampleUsageCount = 0;
};

class BatchConfig {
  public:
    int numBatches;
    int teamsPerBatch;

    std::vector<EquipmentConfig> weaponConfigs;
    std::vector<EquipmentConfig> armorConfigs;

    int GetLowestUsageWeaponIndex();
    int GetLowestUsageArmorIndex();
};

class BatchCreator {

  private:
    static int totalComponent;

  public:
    BatchConfig CreateBatchConfig(int numBatches, int teamsPerBatch, std::vector<Weapon> *weaponList,
                                  std::vector<Armor> *armorList);
    std::vector<Batch> CreateBatches(BatchConfig config);

    int GetRandomEquipmentIndex();

    void SetTotalComponent(int value) {
        totalComponent = value;
    }
};

#endif