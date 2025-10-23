#include "Chromosome.h"

void Chromosome::ApplyStatToComponents(ElementList *componentList) {
    int weaponSize = componentList->weapons.size();
    int armorSize = componentList->armors.size();

    if (isAppliedAll) {
        // Original version: apply same stat to all components
        for (int i = 0; i < weaponSize; i++) {
            componentList->weapons[i].resetResults();
            componentList->weapons[i].correctionStat = &appliedStat_ALL;
        }
        for (int i = 0; i < armorSize; i++) {
            componentList->armors[i].resetResults();
            componentList->armors[i].correctionStat = &appliedStat_ALL;
        }
    } else {
        // New version: apply individual stats to each component
        for (int i = 0; i < weaponSize; i++) {
            componentList->weapons[i].resetResults();
            componentList->weapons[i].correctionStat = &appliedStat_INDIVIDUAL[i];
        }
        for (int i = 0; i < armorSize; i++) {
            componentList->armors[i].resetResults();
            componentList->armors[i].correctionStat = &appliedStat_INDIVIDUAL[weaponSize + i];
        }
    }
}