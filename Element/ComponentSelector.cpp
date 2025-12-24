#include "ComponentSelector.h"
#include "../ExperimentSettings.h"
#include <algorithm>
#include <random>

const float MAX_STRATEGY_RATIO = 0.5f;

ElementList ComponentSelector::SelectRandomCompoent(ElementList elementList, int amount) {

    int max_strategyComponent = amount * MAX_STRATEGY_RATIO;

    if (max_strategyComponent == 0)
        return elementList;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    ElementList selectedList;
    int enemyTargetCount;

    do {
        selectedList.weapons.clear();
        selectedList.armors.clear();

        std::sample(elementList.weapons.begin(), elementList.weapons.end(), std::back_inserter(selectedList.weapons),
                    amount, gen);

        std::sample(elementList.armors.begin(), elementList.armors.end(), std::back_inserter(selectedList.armors),
                    amount, gen);

        enemyTargetCount = std::count_if(selectedList.weapons.begin(), selectedList.weapons.end(), [](Weapon &weapon) {
            return weapon.GetAction().GetTargetType() == TargetType::ENEMY;
        });

    } while (enemyTargetCount < max_strategyComponent);

    return selectedList;
}