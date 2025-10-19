#include "GeneticBalancingProcessor.h"

//statics
Chromosome* GeneticBalancingProcessor::firstChromosome= nullptr;

std::vector<Stat> GeneticBalancingProcessor::firstStats;

float GeneticBalancingProcessor::firstMagnitude;

int GeneticBalancingProcessor::Generation=1;


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

GeneticBalancingProcessor::firstChromosome->averageWinrate = GetAverageWinrate(firstSubjects);

//vector<Stat> filled with zero Stats.
GeneticBalancingProcessor::firstChromosome->appliedStats = EmptyStats(stats.size());
}


// Only for the first chromosome
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

        targetMagnitude += 
        target_ATK * target_ATK + 
        target_DEF * target_DEF + 
        target_HP * target_HP + 
        target_SPD * target_SPD + 
        target_THR * target_THR;
    }

    float mag_float = std::sqrt(static_cast<float>(targetMagnitude));

    return static_cast<float>(dotProduct) / (firstMagnitude * mag_float);
}

void GeneticBalancingProcessor::GenerateBalancingLane()
{
for(int i=0; i< ChromosomeRules_Alpha.size(); i++)
{
    BalancingLane lane = BalancingLane(&ChromosomeRules_Alpha[i], CHROMOSOMETYPE::ALPHA);
    lanes.push_back(lane);
}
for(int i=0; i< ChromosomeRules_Beta.size(); i++)
{
    BalancingLane lane= BalancingLane(&ChromosomeRules_Beta[i], CHROMOSOMETYPE::BETA);
    lanes.push_back(lane);
}
}

std::vector<float> GeneticBalancingProcessor::GetAverageWinrate(std::vector<TestSubject*> testSubjects)
{
    //WEAPON -> ARMOR
std::vector<float> winRate;
for(TestSubject* testSubject : testSubjects)
{
winRate.push_back(testSubject->WinRate);
}
return winRate;
}


void GeneticBalancingProcessor::Init_AlphaBetaChromosome()
{
    //generate first 5 alpha chromosome and 20 beta chromosome.
    //each lane place chromosome(instantiate)
    //TODO:applied stats
   for(BalancingLane& lane: lanes)
   {
    Chromosome* chromosome = new Chromosome(firstChromosome->appliedStats); 
    chromosome->averageWinrate = firstChromosome->averageWinrate;  
    
    std::vector<Stat> appliedStat;
    if(lane.GetChromosomeType() == CHROMOSOMETYPE::ALPHA)
        appliedStat = CreateAppliedStats_ALPHA(firstChromosome, lane.adjustment);
    else // BETA
        appliedStat = CreateAppliedStats_BETA(firstChromosome, lane.adjustment); 

    chromosome->appliedStats = appliedStat;
    lane.SetChromosome(chromosome);
   }
}


int GeneticBalancingProcessor::BuffOrNerf(float winRate)
{
    if (winRate < (TARGET_WINRATE - TARGET_THRESHOLD)) {
        return 1;   
    } 
    else if (winRate > (TARGET_WINRATE + TARGET_THRESHOLD)) {
        return -1; 
    } 
    else {
        return 0;   
    }
}

// Get vector of emptied Stats. 
std::vector<Stat> GeneticBalancingProcessor::EmptyStats(int componentAmount)
{
    std::vector<Stat> stats;

    for(int i=0 ; i<componentAmount; i++)
    {
       Stat stat = Stat(0,0,0,0,0);
       stats.push_back(stat);
    }

    return stats;
}

// only for ALPHA
std::vector<Stat> GeneticBalancingProcessor::CreateAppliedStats_ALPHA(Chromosome* chromosome, const Stat* ruleStat)
{
    std::vector<Stat> appliedStats = EmptyStats(chromosome->averageWinrate.size());

    for(size_t i = 0; i < chromosome->averageWinrate.size(); i++)
    {
       int buffOrNerf = BuffOrNerf(chromosome->averageWinrate[i]);
       Stat scaledStat = (*ruleStat) * buffOrNerf;
       scaledStat += chromosome->appliedStats[i]; 
       appliedStats[i] = scaledStat;
    }

    return appliedStats;
}
// only for BETA
std::vector<Stat> GeneticBalancingProcessor::CreateAppliedStats_BETA(Chromosome* chromosome, const Stat* ruleStat)
{
    std::vector<Stat> appliedStats = EmptyStats(chromosome->averageWinrate.size());

    for(size_t i = 0; i < chromosome->averageWinrate.size(); i++)
    {
       Stat scaledStat = (*ruleStat); 
       scaledStat += chromosome->appliedStats[i];
       appliedStats[i] = scaledStat;
    }

    return appliedStats;
}

// Get Stats from testsubjects and return vector of Stats that are added with applied Stats.
std::vector<Stat> GeneticBalancingProcessor::GetCorrectedStats(std::vector<TestSubject*> mergedTestSubjects)
{
    std::vector<Stat> correctedStats;

    for(TestSubject* testSubject: mergedTestSubjects){
        Stat correctedStat = testSubject->GetStat();
        correctedStat += *(testSubject->correctionStat);
        correctedStats.push_back(correctedStat);
    }

    return correctedStats;
}


void GeneticBalancingProcessor::RunAutoBalancing(Simulator* simulator, std::vector<Batch>* batches)
{
    // Initialization
    GenerateBalancingLane();
    Init_AlphaBetaChromosome();

    // First Generation Setting
    for(BalancingLane& lane: lanes)
    {
        // Simulate lane
        simulator->SimulateBatches(batches, lane.GetChromosome());
        // Get Testsubjects from the simulator
        std::vector<TestSubject*> mergedTestSubjects = MergeToTestSubject(
                simulator->GetElementList()->getWeapons(),  
                simulator->GetElementList()->getArmors()   
        );
        // Get Testsubjects win-rate and set it in chromosome
        lane.GetChromosome()->averageWinrate = GetAverageWinrate(mergedTestSubjects);
        // Calculate and set Chromosome's Root Mean Square Error
        lane.GetChromosome()->Set_RMSE(RMSE::Calculate(mergedTestSubjects));
        // Calculate degree of change and set to it's chromosome
        float degreeOfChange = CosineSimilarity(GetCorrectedStats(mergedTestSubjects));
        lane.GetChromosome()->Set_DegreeOfChange(degreeOfChange);
        // Set fitness of Chromosome
        lane.GetChromosome()->Set_Fitness();
    }
    //Mutation


    // Generation progression
    while(Generation < MAXGENERATION
    //SecondCondition
    )
    {
    Generation++;
    }

}

std::vector<TestSubject*> GeneticBalancingProcessor::MergeToTestSubject(
    std::vector<Weapon>& weapons,   
    std::vector<Armor>& armors)     
{
    std::vector<TestSubject*> mergedSubjects;
    mergedSubjects.reserve(weapons.size() + armors.size());

    for(Weapon& weapon : weapons)  
    {
        mergedSubjects.push_back(&weapon);  
    }

    for(Armor& armor : armors)
    {
        mergedSubjects.push_back(&armor);
    }
    
    return mergedSubjects;  
}


