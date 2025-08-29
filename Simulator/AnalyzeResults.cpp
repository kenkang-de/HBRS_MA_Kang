#include "ResultAnalyzer.h"
#include <iostream>

int main() {
    // Create analyzer with Log/Test directory as log directory
    ResultAnalyzer analyzer("Log/Test/");
    
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
    
    // Save results to CSV file
    analyzer.saveResultsToFile("battle_analysis.csv");
    
    return 0;
}
