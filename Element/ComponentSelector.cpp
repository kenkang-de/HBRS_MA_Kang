#include "ComponentSelector.h"
#include <algorithm>
#include <random>

ElementList ComponentSelector::SelectRandomCompoent(ElementList elementList, int amount) {
    float max_strategyRatio = ExperimentSettings::Default_RATIO_CS > ExperimentSettings::Default_RATIO_SYS
                                  ? ExperimentSettings::Default_RATIO_CS
                                  : ExperimentSettings::Default_RATIO_SYS;

    int max_strategyComponent = amount * max_strategyRatio;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    ElementList selectedList;
    int enemyTargetCount;

    do {
        selectedList.weapons.clear();
        selectedList.armors.clear();

        std::sample(elementList.weapons.begin(), elementList.weapons.end(), std::back_inserter(selectedList.weapons),
                    std::min(amount, static_cast<int>(elementList.weapons.size())), gen);

        std::sample(elementList.armors.begin(), elementList.armors.end(), std::back_inserter(selectedList.armors),
                    std::min(amount, static_cast<int>(elementList.armors.size())), gen);

        enemyTargetCount = std::count_if(selectedList.weapons.begin(), selectedList.weapons.end(), [](Weapon &weapon) {
            return weapon.GetAction().GetTargetType() == TargetType::ENEMY;
        });

    } while (enemyTargetCount < max_strategyComponent);

    return selectedList;
}