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
    float cosineChange = static_cast<float>(dotProduct) / (firstMagnitude * mag_float);
    float inverse = 1.0f - cosineChange;

    return inverse;
}

void GeneticBalancingProcessor::GenerateBalancingLane()
{
 lanes.reserve(ALPHA_NUM + BETA_NUM + 1); // 1 for the mutation. 1 for the reallocation.


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

std::vector<float> GeneticBalancingProcessor::MergeAverageWinrate(std::vector<float> winRateList1, std::vector<float> winRateList2)
{
    std::vector<float> mergedWinrateList;
    mergedWinrateList.reserve(winRateList1.size());

    for(int i=0; i<winRateList1.size(); i++){
        // result to 2 decimal places
                float merged = std::round((winRateList1[i] + winRateList2[i]) / 2.0f * 100.0f) / 100.0f;
        mergedWinrateList.push_back(merged);  // Use push_back, not indexing
    }

    return mergedWinrateList;
}

std::vector<Stat> GeneticBalancingProcessor::MergeAppliedStatLists(std::vector<Stat>& statList1, std::vector<Stat>& statList2)
{
    std::vector<Stat> mergedStatList;
    mergedStatList.reserve(statList1.size());

    for(int i=0; i<statList1.size(); i++)
    {
    Stat mergedStat = statList1[i];  
    mergedStat += statList2[i];   
    mergedStatList.push_back(mergedStat);
    }

    return mergedStatList;
}


void GeneticBalancingProcessor::Init_AlphaBetaChromosome()
{
    //generate first 5 alpha chromosome and 20 beta chromosome.
    //each lane place chromosome(instantiate)
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

std::vector<BalancingLane*> GeneticBalancingProcessor::GetAlphaLanes()
{
std::array<BalancingLane*,ALPHA_NUM> alphaLanes;
for(int i=0; i<ALPHA_NUM; i++)
alphaLanes[i]= &lanes[i];

return std::vector<BalancingLane*>(alphaLanes.begin(), alphaLanes.end());
}

std::vector<BalancingLane*> GeneticBalancingProcessor::GetBetaLanes()
{
    std::array<BalancingLane*,BETA_NUM> betaLanes;
    for(int i=0; i<BETA_NUM; i++)  
        betaLanes[i]= &lanes[ALPHA_NUM+i]; 
    
    return std::vector<BalancingLane*>(betaLanes.begin(), betaLanes.end());
}


std::array<Chromosome*,2> GeneticBalancingProcessor::Pick2RandomLanes(std::vector<BalancingLane*> lanes)
{
    std::array<Chromosome*,2> selectedChromosomes;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, lanes.size() - 1);

    // Pick first random lane
    size_t firstIndex = dist(gen);
    selectedChromosomes[0] = lanes[firstIndex]->GetChromosome();
    
    // Pick second random lane (different from first)
    size_t secondIndex;
    do {
        secondIndex = dist(gen);
    } while (secondIndex == firstIndex && lanes.size() > 1);
    
    selectedChromosomes[1] = lanes[secondIndex]->GetChromosome();

    return selectedChromosomes;
}

std::array<Chromosome*,2> GeneticBalancingProcessor::ParentTournament()
{
    std::array<Chromosome*,2> parentChromosomes;

    // ALPHA Tournament
    std::array<Chromosome*,2> alphaCandidate = Pick2RandomLanes(GetAlphaLanes());
    std::vector<Chromosome*> alphaCandidateVector(alphaCandidate.begin(), alphaCandidate.end());
    Chromosome* alphaParent = GetHighestFitnessChromosome(alphaCandidateVector);

    // BETA Tournament
    std::array<Chromosome*,2> betaCandidate = Pick2RandomLanes(GetBetaLanes());
    std::vector<Chromosome*> betaCandidateVector(betaCandidate.begin(), betaCandidate.end());
    Chromosome* betaParent = GetHighestFitnessChromosome(betaCandidateVector);

    parentChromosomes[0] = alphaParent;
    parentChromosomes[1] = betaParent;

    return parentChromosomes;
}

Chromosome* GeneticBalancingProcessor::PopulateMutation(std::array<Chromosome*,2> parentChromosomes)
{
    // Merge values for 
    // BalancingLane: Stat* adjustment
    // Chromosome: std::vector<Stat> 
    // Chromosome: std::vector<float> averageWinrate

    // BalancingLane
    // Stat adjustment in balancingLane, const value
    Stat alphaAdjustment = *(parentChromosomes[0]->Get_BalancingLane()->adjustment); 
    Stat betaAdjustment = *(parentChromosomes[1]->Get_BalancingLane()->adjustment);
    const Stat* mergedAdjustment = new Stat(alphaAdjustment + betaAdjustment);

    BalancingLane mutationLane(mergedAdjustment, CHROMOSOMETYPE::ALPHA);
    lanes.push_back(mutationLane);

    //Chromosome
    std::vector<Stat> appliedStats = MergeAppliedStatLists(parentChromosomes[0]->appliedStats, parentChromosomes[1]->appliedStats);
    std::vector<float> averageWinrate= MergeAverageWinrate(parentChromosomes[0]->averageWinrate, parentChromosomes[1]->averageWinrate);

    Chromosome* mutatedChromosome = new Chromosome(appliedStats); 
    mutatedChromosome->averageWinrate = averageWinrate;  
    
    //Link Lane and Chromosome
    lanes.back().SetChromosome(mutatedChromosome);

    return mutatedChromosome;

}

Chromosome* GeneticBalancingProcessor::GetHighestFitnessChromosome(std::vector<Chromosome*> chromosomes)
{
    Chromosome* bestChromosome = chromosomes[0];
    float highestFitness = bestChromosome->Get_Fitness();
    
    for(Chromosome* chromosome : chromosomes)
    {
        float currentFitness = chromosome->Get_Fitness();
        if (currentFitness > highestFitness) {
            highestFitness = currentFitness;
            bestChromosome = chromosome;
        }
    }
    
    return bestChromosome;
}

Chromosome* GeneticBalancingProcessor::GetLowestFitnessChromosome(std::vector<Chromosome*> chromosomes)
{
    Chromosome* bestChromosome = chromosomes[0];
    float lowestFitness = bestChromosome->Get_Fitness();
    
    for(Chromosome* chromosome : chromosomes)
    {
        float currentFitness = chromosome->Get_Fitness();
        if (currentFitness < lowestFitness) {
            lowestFitness = currentFitness;
            bestChromosome = chromosome;
        }
    }
    
    return bestChromosome;
}

std::vector<Chromosome*> GeneticBalancingProcessor::GetAllChromosomesFromLanes()
{
    std::vector<Chromosome*> allChromosome;

    for(BalancingLane& lane: lanes)
    {
        allChromosome.push_back(lane.GetChromosome());
    }
    return allChromosome;
}

void GeneticBalancingProcessor::DeleteChromosomeAndLane(Chromosome* chromosome)
{
    // Find the lane that contains this chromosome
    auto it = std::find_if(lanes.begin(), lanes.end(),
        [chromosome](BalancingLane& lane) { 
            return lane.GetChromosome() == chromosome; 
        });
    
    if (it != lanes.end()) {
        // Break the links
        it->SetChromosome(nullptr);
        chromosome->Set_BalancingLane(nullptr);
        
        // Erase by iterator 
        lanes.erase(it);
    }
    
    delete chromosome;
}

void GeneticBalancingProcessor::ReplaceRecessiveChromosome(Chromosome* recessive, Chromosome* mutant)
{
    BalancingLane* mutantLane = mutant->Get_BalancingLane();
    BalancingLane* recessiveLane = recessive->Get_BalancingLane();

    auto it = std::find_if(lanes.begin(), lanes.end(),
        [mutantLane](BalancingLane& lane) { 
            return &lane == mutantLane;  // Compare lane addresses directly
        });

    // Break old links
    mutant->Set_BalancingLane(nullptr);
    recessiveLane->SetChromosome(nullptr);

    // Transfer the adjustment and establish new links
    recessiveLane->adjustment = mutantLane->adjustment;
    recessiveLane->SetChromosome(mutant);
    mutant->Set_BalancingLane(recessiveLane);

    // Remove the mutant lane (found before we changed links)
    if (it != lanes.end()) {
        lanes.erase(it);
    }

    delete recessive;
}

void GeneticBalancingProcessor::SimulateChromosome(Simulator* simulator, 
    std::vector<Batch>* batches, 
    Chromosome* chromosome,
    std::vector<TestSubject*> mergedTestSubjects)
{
    simulator->SimulateBatches(batches, chromosome);
    // Get Testsubjects win-rate and set it in chromosome
    chromosome->averageWinrate = GetAverageWinrate(mergedTestSubjects);
    // Calculate and set Chromosome's Root Mean Square Error
    chromosome->Set_RMSE(RMSE::Calculate(mergedTestSubjects));
    // Calculate degree of change and set to it's chromosome
    float degreeOfChange = CosineSimilarity(GetCorrectedStats(mergedTestSubjects));
    chromosome->Set_DegreeOfChange(degreeOfChange);
    // Set fitness of Chromosome
    chromosome->Set_Fitness();

}

void GeneticBalancingProcessor::RunAutoBalancing(Simulator* simulator, std::vector<Batch>* batches)
{
    // Initialization
    GenerateBalancingLane();
    Init_AlphaBetaChromosome();

    // Get Testsubjects from the simulator
    std::vector<TestSubject*> mergedTestSubjects = MergeToTestSubject(
            simulator->GetElementList()->getWeapons(),  
            simulator->GetElementList()->getArmors()   
    );

    // First Generation Setting
    // Simulate lane
    for(BalancingLane& lane: lanes)
    SimulateChromosome(simulator,batches,lane.GetChromosome(),mergedTestSubjects);
    
    // Generation progression
    while(Generation < MAXGENERATION &&
    // If the highest fitness score among all lanes has reached the threshold
    GetHighestFitnessChromosome(GetAllChromosomesFromLanes())->Get_Fitness() <= FITNESS_MAX-FITNESS_THRESHOLD){
    //Mutation
    std::array<Chromosome*,2> parentCandidates = ParentTournament();
    Chromosome* mutantChromosome = PopulateMutation(parentCandidates);
    //Simulate Mutation
    SimulateChromosome(simulator,batches,mutantChromosome,mergedTestSubjects);
    //Selection
    Chromosome* recessiveChromosome = GetLowestFitnessChromosome(GetAllChromosomesFromLanes()); 

    if(recessiveChromosome == mutantChromosome)
    DeleteChromosomeAndLane(recessiveChromosome);
    else
    ReplaceRecessiveChromosome(recessiveChromosome, mutantChromosome);

    std::cout<<"Generation: " <<Generation<<std::endl;

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


