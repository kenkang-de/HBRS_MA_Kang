#include <iostream>  
#include <vector>  

#include "Simulator.h"
#include "Constants.h" 
#include "BattleManager.h"


void Simulator::Init_Battlefield()
{
    std::array<Unit*,5> redTeamPtrs;
    std::array<Unit*,5> blueTeamPtrs;

    for (int i = 0; i < UNITS_PER_TEAM; i++) {
        redTeamPtrs[i] = &(*battleUnits)[i];     
        blueTeamPtrs[i] = &(*battleUnits)[i + UNITS_PER_TEAM]; 
    }

    battlefield = std::make_unique<Battlefield>(redTeamPtrs, blueTeamPtrs);
}


void Simulator::SimulateBatches(std::vector<Batch>* batches)
{
std::cout<<"SimulateBatches: Starting with "<<batches->size()<<" batches"<<std::endl;
//Iterate through batches, teams
for(Batch& batch : *batches){
    std::cout<<"Processing batch: "<<batch.batchId<<" with "<<batch.teams.size()<<" teams"<<std::endl;
    // Round-Robin Tournament: Each team plays against every other team
    for(int teamA = 0; teamA < batch.teams.size(); teamA++){
        for(int teamB = teamA + 1; teamB < batch.teams.size(); teamB++){
            std::cout<<"Battle: Team "<<teamA<<" vs Team "<<teamB<<std::endl;
            //First battle of the batch will be logged, and saved to Log directory
            SimulateBattle(&batch.teams[teamA], &batch.teams[teamB], teamA==0 && teamB==1,batch.batchId);
            std::cout<<"Battle completed!"<<std::endl;
        }}}
std::cout<<"SimulateBatches: All batches completed!"<<std::endl;
}

void Simulator::EquipTeam(int startIndex, SimulationTeamSetting* teamSetting)
{
    for(int i = 0; i < UNITS_PER_TEAM; i++)
    {
        Unit* unit = &(*battleUnits)[startIndex + i];
        SimulationUnitSetting& unitSetting = teamSetting->simulationUnitSettings[i];

        unit->ResetUnit();
        
        Weapon* weapon = elementList->FindWeaponByID(unitSetting.weaponID);
        if(weapon) {
            unit->SetWeapon(weapon);
        }
        Armor* armor = elementList->FindArmorByID(unitSetting.armorID);
        if(armor) {
            unit->SetArmor(armor);
        }
    }
}

void Simulator::SimulateBattle(SimulationTeamSetting* redTeamSetting, SimulationTeamSetting* blueTeamSetting, bool log, std::string batchID)
{
    std::cout<<"[SimulateBattle] Starting battle setup..."<<std::endl;
    
    // Equip both teams
    std::cout<<"[SimulateBattle] Equipping red team..."<<std::endl;
    EquipTeam(0, redTeamSetting);  
    std::cout<<"[SimulateBattle] Red team equipped"<<std::endl;
    
    std::cout<<"[SimulateBattle] Equipping blue team..."<<std::endl;
    EquipTeam(UNITS_PER_TEAM, blueTeamSetting); 
    std::cout<<"[SimulateBattle] Blue team equipped"<<std::endl;
    
    std::cout<<"[SimulateBattle] Creating BattleManager..."<<std::endl;
    BattleManager battleManager(*battlefield);
    std::cout<<"[SimulateBattle] BattleManager created successfully"<<std::endl;
    
    std::cout<<"[SimulateBattle] Starting battle with log="<<(log?"true":"false")<<", batchID="<<batchID<<std::endl;
    battleManager.StartBattle(log,batchID);
    std::cout<<"[SimulateBattle] Battle completed successfully!"<<std::endl;
}




// int main(int argc, char* argv[])
// {    

    // Load existing TestSubject statistics from previous simulations
    //TestSubjectPersistence::loadTestSubjectData();
    
    // Variables to capture selected simulation output
    // std::ostringstream selectedOutput;
    // std::ostringstream nullOutput; // For suppressing non-selected simulation output
    // bool outputToFile = false;
    
    // // Run multiple simulations
    // for (int sim = 0; sim < numSimulations; sim++) {
    //     // Determine if this is the selected simulation to output
    //     outputToFile = (sim + 1 == selectedSim);
        
    //     // Redirect cout output - to selected stream for chosen sim, to null stream for others
    //     std::streambuf* coutBuf = std::cout.rdbuf(); // Save original cout buffer
    //     if (outputToFile) {
    //         std::cout.rdbuf(selectedOutput.rdbuf()); // Redirect cout to string stream for selected sim
    //     } else {
    //         std::cout.rdbuf(nullOutput.rdbuf()); // Redirect cout to null stream for non-selected sims
    //     }
        
    //     // Show progress for non-selected simulations
    //     if (!outputToFile && numSimulations > 1) {
    //         // Temporarily restore cout to show progress
    //         std::cout.rdbuf(coutBuf);
    //         std::cout << "Running simulation " << (sim + 1) << "/" << numSimulations << "..." << std::endl;
    //         std::cout.rdbuf(nullOutput.rdbuf()); // Redirect back to null
    //     }
        
    //     // Reseed random generator for different results each run
    //     ReseedRandomGenerator();
        
    //     std::list<Unit> allUnits = GenerateUnits(10);

    //     auto actionMap = LoadActionsFromYAML("BattleActions.yaml");

    //     // Try to load from configuration, fallback to random if not available
    //     std::string configFile = "battle_configs.csv";
    //     bool useConfig = EquipUnitsFromConfig(allUnits, configFile, sim);
        
    //     if (!useConfig) {
    //         std::cout << "[Simulator] Config loading failed, using random equipment" << std::endl;
    //         EquipUnitsRandomEquipments(allUnits);
    //     }



    //     // Create Battlefield with unit vectors (it will create teams internally)
    //     Battlefield battlefield(redPtrs, bluePtrs);
        
    //     // Create BattleManager with just the Battlefield
    //     BattleManager battleManager(battlefield);
    //     battleManager.StartBattle();
        
    //     // After battle ends, record equipment usage using TestSubject structure
    //     Team* winner = battlefield.ResultCheck();
    //     bool redWon = (winner && winner->GetTeamColor() == Red);
    //     bool blueWon = (winner && winner->GetTeamColor() == Blue);
    //     bool isDraw = !redWon && !blueWon;
        
    //     // Record equipment usage for each unit using the actual Weapon/Armor TestSubject data
    //     for (Unit* unit : allUnitPtrs) {
    //         if (!unit) continue;
            
    //         bool unitWon = (unit->team == Red && redWon) || (unit->team == Blue && blueWon);
            
    //         // Update weapon stats directly in the Weapon object
    //         if (unit->GetWeapon()) {
    //             const std::string weaponId = unit->GetWeapon()->GetID();
    //             Weapon* weapon = equipmentManager.getMutableWeaponById(weaponId);
    //             if (weapon) {
    //                 weapon->incrementUsage();
    //                 if (unitWon) weapon->recordWin();
    //                 else if (isDraw) weapon->recordDraw();
    //                 else weapon->recordLoss();
    //             }
    //         }
            
    //         // Update armor stats directly in the Armor object
    //         if (unit->GetArmor()) {
    //             const std::string armorId = unit->GetArmor()->GetID();
    //             Armor* armor = equipmentManager.getMutableArmorById(armorId);
    //             if (armor) {
    //                 armor->incrementUsage();
    //                 if (unitWon) armor->recordWin();
    //                 else if (isDraw) armor->recordDraw();
    //                 else armor->recordLoss();
    //             }
    //         }
    //     }
        
    //     // Restore cout buffer after each simulation
    //     std::cout.rdbuf(coutBuf);
    // }
    
    // // Write the selected simulation output to file
    // std::string logFileName = "Log/V1/V1test" + std::to_string(selectedSim) + ".txt";
    // std::ofstream logFile(logFileName);
    // logFile << selectedOutput.str();
    // logFile.close();
    
    // std::cout << "Saved selected simulation output to: " << logFileName << std::endl;
    
    // // Output results in CSV format using actual Weapon/Armor TestSubject data
    // std::cout << "EQUIPMENT_ANALYSIS_START" << std::endl;
    // std::cout << "Type,ID,WinRate,TotalWin,TotalLost,TotalDraw,UsageCount" << std::endl;
    
    // // Get all weapons and sort by win rate
    // std::vector<Weapon*> sortedWeapons;
    // for (Weapon& weapon : equipmentManager.getAllWeapons()) {
    //     if (weapon.UsageCount > 0) {
    //         sortedWeapons.push_back(&weapon);
    //     }
    // }
    // std::sort(sortedWeapons.begin(), sortedWeapons.end(), 
    //           [](const Weapon* a, const Weapon* b) { return a->WinRate > b->WinRate; });
    
    // for (const Weapon* weapon : sortedWeapons) {
    //     std::cout << "Weapon," << weapon->GetID() << "," 
    //               << std::fixed << std::setprecision(0) << (weapon->WinRate * 100) << ","
    //               << weapon->TotalWin << "," << weapon->TotalLost << "," 
    //               << weapon->TotalDraw << "," << weapon->UsageCount << std::endl;
    // }
    
    // // Get all armor and sort by win rate
    // std::vector<Armor*> sortedArmor;
    // for (Armor& armor : equipmentManager.getAllArmor()) {
    //     if (armor.UsageCount > 0) {
    //         sortedArmor.push_back(&armor);
    //     }
    // }
    // std::sort(sortedArmor.begin(), sortedArmor.end(), 
    //           [](const Armor* a, const Armor* b) { return a->WinRate > b->WinRate; });
    
    // for (const Armor* armor : sortedArmor) {
    //     std::cout << "Armor," << armor->GetID() << "," 
    //               << std::fixed << std::setprecision(0) << (armor->WinRate * 100) << ","
    //               << armor->TotalWin << "," << armor->TotalLost << "," 
    //               << armor->TotalDraw << "," << armor->UsageCount << std::endl;
    // }
    
    // std::cout << "EQUIPMENT_ANALYSIS_END" << std::endl;
    
    // // Save accumulated TestSubject statistics for next simulation
    // TestSubjectPersistence::saveTestSubjectData();
    
    // return 0;
// }