#ifndef BALANCINGRULE_H
#define BALANCINGRULE_H

#include <array>
#include <random>
#include <vector>

#include "../Element/Stat.h"
#include "../Simulator/Constants.h"

class BalancingRule {
  private:
    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_int_distribution<int> dist;

  public:
    static std::vector<Stat> GenerateRamdomAppliedStats(int amount);
    static Stat GenerateRamdomAppliedStat();
};

#endif