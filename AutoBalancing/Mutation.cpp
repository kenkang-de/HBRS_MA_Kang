#include "Mutation.h"
#include "../Constants.h"
#include <algorithm>
#include <random>

void Mutation::GaussianMutation(std::vector<Chromosome *> chromosomeList) {
    // Static random generators for efficiency
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> mutationProb(0.0f, 1.0f);

    // Iterate through all chromosomes except the first one (elite chromosome)
    // Elites are exempt from both crossover and mutation.
    for (size_t i = 1; i < chromosomeList.size(); ++i) {
        Chromosome *chromosome = chromosomeList[i];
        if (mutationProb(gen) < MUTATION_PROBABILITY) {
            if (chromosome->IsAppliedAll()) {
                // Original version: mutate single stat
                MutateStatObject(chromosome->appliedStat_ALL);
            } else {
                // New version: mutate individual stats
                for (Stat &stat : chromosome->appliedStat_INDIVIDUAL) {
                    if (mutationProb(gen) < MUTATION_PROBABILITY) {
                        MutateStatObject(stat);
                    }
                }
            }
        }
    }
}

void Mutation::MutateStatObject(Stat &stat) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<float> gaussianNoise(0.0f, MUTATION_SIGMA);

    // Attack
    int currentAttack = stat.GetAttack();
    float noise = gaussianNoise(gen);
    int newAttack = currentAttack + static_cast<int>(std::round(noise));
    stat.SetAttack(newAttack);

    // Defense
    int currentDefense = stat.GetDefense();
    noise = gaussianNoise(gen);
    int newDefense = currentDefense + static_cast<int>(std::round(noise));
    stat.SetDefense(newDefense);

    // HP
    int currentHP = stat.GetHP();
    noise = gaussianNoise(gen);
    int newHP = currentHP + static_cast<int>(std::round(noise));
    stat.SetHP(newHP);

    // Speed
    int currentSpeed = stat.GetSpeed();
    noise = gaussianNoise(gen);
    int newSpeed = currentSpeed + static_cast<int>(std::round(noise));
    stat.SetSpeed(newSpeed);

    // Threat
    int currentThreat = stat.GetThreat();
    noise = gaussianNoise(gen);
    int newThreat = currentThreat + static_cast<int>(std::round(noise));
    stat.SetThreat(newThreat);
}