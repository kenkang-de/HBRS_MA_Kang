#include "ResultAnalyzer.h"
#include "../Paths.h"
#include <iostream>

int main() {
    // Create analyzer with Log/V1 directory as log directory
    ResultAnalyzer analyzer(Paths::FromAnalysis::LOG_V1_DIR);
    
    std::cout << "Battle Result Analyzer" << std::endl;
    std::cout << "=====================" << std::endl << std::endl;
    
    // Analyze V1Test1 through V1Test10 (post-defense nerf tests)
    std::cout << "Analyzing V1Test1 through V1Test10..." << std::endl;
    analyzer.analyzeTestRange("V1Test", 1, 10);
    
    std::cout << std::endl;
    
    // Print all individual results
    analyzer.printAllResults();
    
    // Print overall statistics
    analyzer.printStatistics();
    
    // Save results to CSV file in Log/V1 directory
    analyzer.saveResultsToFile(Paths::FromAnalysis::LOG_V1_DIR + Paths::BATTLE_ANALYSIS_CSV);
    
    return 0;
}
