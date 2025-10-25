#include "BalancingRule.h"

std::random_device BalancingRule::rd;
std::mt19937 BalancingRule::gen(BalancingRule::rd());
std::uniform_int_distribution<int> BalancingRule::dist(1, APPLIEDSTAT_RANGE);

std::vector<Stat> BalancingRule::GenerateRamdomAppliedStats(int amount) {

    std::vector<Stat> appliedStats;

    for (int i = 0; i < amount; i++) {
        int randAttack = dist(gen);
        int randDefense = static_cast<int>(dist(gen) * 0.5f);
        int randHP = dist(gen);
        int randSpeed = static_cast<int>(dist(gen) * 0.2f);
        int randThreat = dist(gen);

        appliedStats.push_back(Stat(randAttack, randDefense, randHP, randSpeed, randThreat));
    }

    return appliedStats;
}

Stat BalancingRule::GenerateRamdomAppliedStat() {

    int randAttack = dist(gen);
    int randDefense = static_cast<int>(dist(gen) * 0.5f);
    int randHP = dist(gen);
    int randSpeed = static_cast<int>(dist(gen) * 0.2f);
    int randThreat = dist(gen);

    return Stat(randAttack, randDefense, randHP, randSpeed, randThreat);
}
