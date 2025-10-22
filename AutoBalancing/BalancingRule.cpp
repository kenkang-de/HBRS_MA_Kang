#include "BalancingRule.h"
#include "../Simulator/Constants.h"

std::random_device BalancingRule::rd;
std::mt19937 BalancingRule::gen(BalancingRule::rd());
std::uniform_int_distribution<int> BalancingRule::dist(-APPLIEDSTAT_RANGE, APPLIEDSTAT_RANGE);

std::vector<Stat> BalancingRule::GenerateRamdomAppliedStats(int amount) {

    std::vector<Stat> appliedStats;

    for (int i = 0; i < amount; i++) {
        int randAttack = dist(gen);
        int randDefense = dist(gen);
        int randHP = dist(gen);
        int randSpeed = dist(gen);
        int randThreat = dist(gen);

        appliedStats.push_back(Stat(randAttack, randDefense, randHP, randSpeed, randThreat));
    }

    return appliedStats;
}

Stat BalancingRule::GenerateRamdomAppliedStat() {

    int randAttack = dist(gen);
    int randDefense = dist(gen);
    int randHP = dist(gen);
    int randSpeed = dist(gen);
    int randThreat = dist(gen);

    return Stat(randAttack, randDefense, randHP, randSpeed, randThreat);
}
