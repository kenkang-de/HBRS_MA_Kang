#include "ElementAnalysis.h"
#include "../Paths.h"
#include <iostream>

int main() {
    std::cout << "=== Equipment Performance Analyzer ===" << std::endl;
    std::cout << "======================================" << std::endl << std::endl;
    
    // Get the element analysis instance (should contain data from simulations)
    ElementAnalysis* analyzer = ElementAnalysis::getInstance();
    
    // Generate the analysis report
    std::string outputPath = Paths::FromAnalysis::LOG_V1_DIR + "ElementAnalysisV1_Report.csv";
    analyzer->generateAnalysisReport(outputPath);
    
    std::cout << std::endl << "Equipment analysis complete!" << std::endl;
    std::cout << "Results saved to: " << outputPath << std::endl;
    
    return 0;
}
