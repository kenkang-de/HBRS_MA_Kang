#include "GeneticBalancingProcessor.h"

//statics
Chromosome* GeneticBalancingProcessor::firstChromosome= nullptr;

std::vector<Stat> GeneticBalancingProcessor::firstStats;

float GeneticBalancingProcessor::firstMagnitude;

//[ORDER] always weapon first!
void GeneticBalancingProcessor::GenerateFirstChromosome(ElementList *elementList, std::vector<TestSubject*> firstSubjects)
{
std::vector<Stat> stats;

for(Weapon& weapon: elementList->weapons)
{
stats.push_back(weapon.GetStat());
}

for(Armor& armor: elementList->armors)
{
stats.push_back(armor.GetStat());
}

GeneticBalancingProcessor::firstStats = stats;

GeneticBalancingProcessor::firstMagnitude = CalcStatMagnitude(stats);

GeneticBalancingProcessor::firstChromosome = new Chromosome(RMSE::Calculate(firstSubjects));
}

int GeneticBalancingProcessor::CalcStatMagnitude(std::vector<Stat> stats)
{
    int magnitude = 0;

    for (const Stat& stat : stats) {
        int attack = stat.GetAttack();
        int defense = stat.GetDefense();
        int hp = stat.GetHP();
        int speed = stat.GetSpeed();
        int threat = stat.GetThreat();
        
        magnitude += attack * attack + defense * defense + hp * hp + speed * speed + threat * threat;
    }

    return magnitude;
}

float GeneticBalancingProcessor::CosineSimilarity(std::vector<Stat> targetStats)
{
int dotProduct=0;
int targetMagnitude=0;

    for (size_t i = 0; i < firstStats.size(); ++i) {
        int target_ATK = targetStats[i].GetAttack();
        int target_DEF =  targetStats[i].GetDefense();
        int target_HP =  targetStats[i].GetHP();
        int target_SPD =  targetStats[i].GetSpeed();
        int target_THR =  targetStats[i].GetThreat();
        
        dotProduct += firstStats[i].GetAttack() * target_ATK + 
                     firstStats[i].GetDefense()  * target_DEF + 
                     firstStats[i].GetHP() * target_HP + 
                     firstStats[i].GetSpeed()  * target_SPD + 
                     firstStats[i].GetThreat()  * target_THR;

        targetMagnitude += target_ATK * target_ATK + target_DEF * target_DEF + target_HP * target_HP + target_SPD * target_SPD + target_THR * target_THR;
    }

    float mag_float = std::sqrt(static_cast<float>(targetMagnitude));

    return static_cast<float>(dotProduct) / (firstMagnitude * mag_float);
}
