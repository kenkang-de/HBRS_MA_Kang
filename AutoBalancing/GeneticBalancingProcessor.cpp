#include "GeneticBalancingProcessor.h"
#include "Crossover.h"
#include "Mutation.h"

// statics
Chromosome *GeneticBalancingProcessor::firstChromosome = nullptr;

std::vector<Stat> GeneticBalancingProcessor::firstStats;

float GeneticBalancingProcessor::firstMagnitude;

int GeneticBalancingProcessor::Generation = 1;

//[ORDER] always weapon first!
void GeneticBalancingProcessor::GenerateFirstChromosome(ElementList *elementList,
                                                        std::vector<TestSubject *> firstSubjects) {
    std::vector<Stat> stats;

    for (Weapon &weapon : elementList->weapons) {
        stats.push_back(weapon.GetStat());
    }

    for (Armor &armor : elementList->armors) {
        stats.push_back(armor.GetStat());
    }

    GeneticBalancingProcessor::firstStats = stats;

    GeneticBalancingProcessor::firstMagnitude = CalcStatMagnitude(stats);

    GeneticBalancingProcessor::firstChromosome = new Chromosome(RMSE::Calculate(firstSubjects));

    GeneticBalancingProcessor::firstChromosome->averageWinrates = GetAverageWinrate(firstSubjects);

    // vector<Stat> filled with zero Stats.
    GeneticBalancingProcessor::firstChromosome->appliedStat_INDIVIDUAL = EmptyStats(stats.size());
}

// Only for the first chromosome
int GeneticBalancingProcessor::CalcStatMagnitude(std::vector<Stat> stats) {
    int magnitude = 0;

    for (const Stat &stat : stats) {
        int attack = stat.GetAttack();
        int defense = stat.GetDefense();
        int hp = stat.GetHP();
        int speed = stat.GetSpeed();
        int threat = stat.GetThreat();

        magnitude += attack * attack + defense * defense + hp * hp + speed * speed + threat * threat;
    }

    return magnitude;
}

float GeneticBalancingProcessor::CosineSimilarity(std::vector<Stat> targetStats) {
    int dotProduct = 0;
    int targetMagnitude = 0;

    for (size_t i = 0; i < firstStats.size(); ++i) {
        int target_ATK = targetStats[i].GetAttack();
        int target_DEF = targetStats[i].GetDefense();
        int target_HP = targetStats[i].GetHP();
        int target_SPD = targetStats[i].GetSpeed();
        int target_THR = targetStats[i].GetThreat();

        dotProduct += firstStats[i].GetAttack() * target_ATK + firstStats[i].GetDefense() * target_DEF +
                      firstStats[i].GetHP() * target_HP + firstStats[i].GetSpeed() * target_SPD +
                      firstStats[i].GetThreat() * target_THR;

        targetMagnitude += target_ATK * target_ATK + target_DEF * target_DEF + target_HP * target_HP +
                           target_SPD * target_SPD + target_THR * target_THR;
    }

    float mag_float = std::sqrt(static_cast<float>(targetMagnitude));
    float cosineSimilarity =
        static_cast<float>(dotProduct) / (std::sqrt(static_cast<float>(firstMagnitude)) * mag_float);
    float similarityScore = (cosineSimilarity + 1.0f) / 2.0f;

    return std::round(similarityScore * 1000.0f) / 1000.0f;
}

std::vector<float> GeneticBalancingProcessor::GetAverageWinrate(std::vector<TestSubject *> testSubjects) {
    std::vector<float> winRate;
    for (TestSubject *testSubject : testSubjects) {
        winRate.push_back(testSubject->WinRate);
    }
    return winRate;
}

std::vector<Chromosome *> GeneticBalancingProcessor::Instantiate_FirstGenChromosomes() {

    std::vector<Chromosome *> firstGenChromosomes;
    firstGenChromosomes.reserve(INDIVIDUALS_PER_GENERATION);

    for (int i = 0; i < INDIVIDUALS_PER_GENERATION; i++) {
        Chromosome *newChromosome = new Chromosome(BalancingRule::GenerateRamdomAppliedStats(firstStats.size()));
        firstGenChromosomes.push_back(newChromosome);
    }

    return firstGenChromosomes;
}

int GeneticBalancingProcessor::BuffOrNerf(float winRate) {
    if (winRate < (TARGET_WINRATE - TARGET_THRESHOLD)) {
        return 1;
    } else if (winRate > (TARGET_WINRATE + TARGET_THRESHOLD)) {
        return -1;
    } else {
        return 0;
    }
}

// Get vector of emptied Stats.
std::vector<Stat> GeneticBalancingProcessor::EmptyStats(int componentAmount) {
    std::vector<Stat> stats;

    for (int i = 0; i < componentAmount; i++) {
        Stat stat = Stat(0, 0, 0, 0, 0);
        stats.push_back(stat);
    }

    return stats;
}

std::vector<TestSubject *> GeneticBalancingProcessor::CombineToTestSubject(std::vector<Weapon> &weapons,
                                                                           std::vector<Armor> &armors) {
    std::vector<TestSubject *> mergedSubjects;
    mergedSubjects.reserve(weapons.size() + armors.size());

    for (Weapon &weapon : weapons) {
        mergedSubjects.push_back(&weapon);
    }

    for (Armor &armor : armors) {
        mergedSubjects.push_back(&armor);
    }

    return mergedSubjects;
}

// Get Stats from testsubjects and return vector of Stats that are added with applied Stats.
std::vector<Stat> GeneticBalancingProcessor::GetCorrectedStats(std::vector<TestSubject *> combinedTestSubjects) {
    std::vector<Stat> correctedStats;

    for (TestSubject *testSubject : combinedTestSubjects) {
        Stat correctedStat = testSubject->GetStat();
        correctedStat += *(testSubject->correctionStat);
        correctedStats.push_back(correctedStat);
    }

    return correctedStats;
}

std::array<Chromosome *, 2> GeneticBalancingProcessor::Pick2RandomLanes(std::vector<Chromosome *> chromosomeList) {
    std::array<Chromosome *, 2> selectedChromosomes;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, chromosomeList.size() - 1);

    // Pick first random lane
    size_t firstIndex = dist(gen);
    selectedChromosomes[0] = chromosomeList[firstIndex];

    // Pick second random lane (different from first)
    size_t secondIndex;
    do {
        secondIndex = dist(gen);
    } while (secondIndex == firstIndex && chromosomeList.size() > 1);

    selectedChromosomes[1] = chromosomeList[secondIndex];

    return selectedChromosomes;
}

std::array<Chromosome *, 2> GeneticBalancingProcessor::ParentTournament(std::vector<Chromosome *> chromosomeList) {
    std::array<Chromosome *, 2> parentChromosomes;

    std::array<Chromosome *, 2> parent1 = Pick2RandomLanes(chromosomeList);
    parentChromosomes[0] = GetHighestFitnessChromosome(parent1);

    std::array<Chromosome *, 2> parent2 = Pick2RandomLanes(chromosomeList);
    parentChromosomes[1] = GetHighestFitnessChromosome(parent2);

    return parentChromosomes;
}

Chromosome *GeneticBalancingProcessor::GetHighestFitnessChromosome(std::vector<Chromosome *> chromosomes) {
    Chromosome *bestChromosome = chromosomes[0];
    float highestFitness = bestChromosome->Get_Fitness();

    for (Chromosome *chromosome : chromosomes) {
        float currentFitness = chromosome->Get_Fitness();
        if (currentFitness > highestFitness) {
            highestFitness = currentFitness;
            bestChromosome = chromosome;
        }
    }

    return bestChromosome;
}

Chromosome *GeneticBalancingProcessor::GetHighestFitnessChromosome(std::array<Chromosome *, 2> &chromosomes) {
    return (chromosomes[0]->Get_Fitness() >= chromosomes[1]->Get_Fitness()) ? chromosomes[0] : chromosomes[1];
}

Chromosome *GeneticBalancingProcessor::GetLowestFitnessChromosome(std::vector<Chromosome *> chromosomes) {
    Chromosome *bestChromosome = chromosomes[0];
    float lowestFitness = bestChromosome->Get_Fitness();

    for (Chromosome *chromosome : chromosomes) {
        float currentFitness = chromosome->Get_Fitness();
        if (currentFitness < lowestFitness) {
            lowestFitness = currentFitness;
            bestChromosome = chromosome;
        }
    }

    return bestChromosome;
}

Chromosome *GeneticBalancingProcessor::GetLowestFitnessChromosome(std::array<Chromosome *, 2> &chromosomes) {
    return (chromosomes[0]->Get_Fitness() <= chromosomes[1]->Get_Fitness()) ? chromosomes[0] : chromosomes[1];
}

void GeneticBalancingProcessor::SimulateChromosome(Simulator *simulator, std::vector<Batch> *batches,
                                                   Chromosome *chromosome,
                                                   std::vector<TestSubject *> combinedTestSubjects) {
    simulator->SimulateBatches(batches, chromosome);
    // Get Testsubjects win-rate and set it in chromosome
    chromosome->averageWinrates = GetAverageWinrate(combinedTestSubjects);
    // Calculate and set Chromosome's Root Mean Square Error
    chromosome->Set_RMSE(RMSE::Calculate(combinedTestSubjects));
    // Calculate degree of change and set to it's chromosome
    float degreeOfChange = CosineSimilarity(GetCorrectedStats(combinedTestSubjects));
    chromosome->Set_DegreeOfChange(degreeOfChange);
    // Set fitness of Chromosome
    chromosome->Set_Fitness();
}

void GeneticBalancingProcessor::RunAutoBalancing(Simulator *simulator, BatchConfig *batchConfig) {
    // Initialization
    // First Generation Setting
    currentGenChromosomeList.reserve(INDIVIDUALS_PER_GENERATION);
    nextGenChromosomeList.reserve(INDIVIDUALS_PER_GENERATION);
    currentGenChromosomeList = Instantiate_FirstGenChromosomes();

    BatchCreator batchCreator;
    std::vector<Batch> batches = batchCreator.CreateBatches(*batchConfig);

    // Get Testsubjects from the simulator
    std::vector<TestSubject *> combinedTestSubjects =
        CombineToTestSubject(simulator->GetElementList()->getWeapons(), simulator->GetElementList()->getArmors());

    // Simulate first generation Chromosomes
    for (Chromosome *firstGenchromosome : currentGenChromosomeList) {
        SimulateChromosome(simulator, &batches, firstGenchromosome, combinedTestSubjects);
        BalancingLog(Generation, firstGenchromosome->Get_Fitness(), firstGenchromosome->Get_MRSE(),
                     firstGenchromosome->Get_DOG());
    }

    // Generation Progression
    // Second to max generation
    while (Generation < MAXGENERATION &&
           // If the highest fitness score among all chromosomes has reached the threshold
           GetHighestFitnessChromosome(currentGenChromosomeList)->Get_Fitness() <= FITNESS_MAX - FITNESS_THRESHOLD) {

        std::cout << "[Generation:" << Generation << "]" << std::endl;

        for (Chromosome *currentGenChromosome : currentGenChromosomeList) {
            currentGenChromosome->Set_Survived(false);
        }

        // Elite Chromosome (does not have offspring, progress to next generation)
        Chromosome *eliteChromosome = GetHighestFitnessChromosome(currentGenChromosomeList);
        Chromosome *eliteCopy = new Chromosome(eliteChromosome->appliedStat_INDIVIDUAL);
        eliteCopy->averageWinrates = eliteChromosome->averageWinrates;

        eliteCopy->Set_RMSE(eliteChromosome->Get_MRSE() * RMSE_WEIGHT);
        eliteCopy->Set_DegreeOfChange(eliteChromosome->Get_DOG() / DOC_WEIGHT);
        eliteCopy->Set_Fitness();

        nextGenChromosomeList.push_back(eliteCopy);

        // Crossover
        while (nextGenChromosomeList.size() < INDIVIDUALS_PER_GENERATION) {
            std::array<Chromosome *, 2> parentCandidates = ParentTournament(currentGenChromosomeList);

            // Random crossover probability: CROSSOVER_PROBABILITY = crossover, else = no crossover
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<float> prob(0.0f, 1.0f);

            if (prob(gen) < CROSSOVER_PROBABILITY) {
                std::array<Chromosome *, 2> crossedParents = Crossover::SinglePointCrossOver(parentCandidates);
                nextGenChromosomeList.push_back(crossedParents[0]);
                if (nextGenChromosomeList.size() < INDIVIDUALS_PER_GENERATION) {
                    nextGenChromosomeList.push_back(crossedParents[1]);
                } else {
                    // Population is full, delete the unused second child to prevent memory leak
                    delete crossedParents[1];
                }
            } else {
                Chromosome *highest = GetHighestFitnessChromosome(parentCandidates);
                Chromosome *highestCopy = new Chromosome(highest->appliedStat_INDIVIDUAL);
                highestCopy->averageWinrates = highest->averageWinrates;
                nextGenChromosomeList.push_back(highestCopy);

                if (nextGenChromosomeList.size() < INDIVIDUALS_PER_GENERATION) {
                    Chromosome *lowest = GetLowestFitnessChromosome(parentCandidates);
                    Chromosome *lowestCopy = new Chromosome(lowest->appliedStat_INDIVIDUAL);
                    lowestCopy->averageWinrates = lowest->averageWinrates;
                    nextGenChromosomeList.push_back(lowestCopy);
                }
            }
        }

        std::cout << "Individual Count: " << nextGenChromosomeList.size() << std::endl;

        // Mutation
        Mutation::GaussianMutation(nextGenChromosomeList);

        batches = batchCreator.CreateBatches(*batchConfig);

        // Simulate NextGenerations
        for (Chromosome *nextGenChromosome : nextGenChromosomeList) {
            SimulateChromosome(simulator, &batches, nextGenChromosome, combinedTestSubjects);
            BalancingLog(Generation + 1, nextGenChromosome->Get_Fitness(), nextGenChromosome->Get_MRSE(),
                         nextGenChromosome->Get_DOG());
        }

        // Cleanup all chromosomes since we create copies for survivors
        for (Chromosome *currentGenChromosome : currentGenChromosomeList) {
            delete currentGenChromosome;
        }

        currentGenChromosomeList = nextGenChromosomeList;
        nextGenChromosomeList.clear();

        Generation++;
    }
}
