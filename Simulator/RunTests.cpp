#include "TestRunner.h"
#include <iostream>

int main() {
    std::cout << "=== Simulator Test Suite Runner ===" << std::endl;
    std::cout << "====================================" << std::endl << std::endl;
    
    // Create test runner with default settings
    // This will run Simulator.exe and save logs to Log/V1/V1test1.txt through V1test10.txt
    TestRunner runner;
    
    // Clean old test files
    runner.cleanOldTests(1, 10);
    
    // Run the standard test suite (10 tests)
    if (runner.runStandardTestSuite()) {
        std::cout << std::endl << "All tests completed successfully!" << std::endl;
        std::cout << "Results saved in Log/V1/ directory" << std::endl;
    } else {
        std::cout << std::endl << "Some tests failed. Check the output above for details." << std::endl;
        return 1;
    }
    
    return 0;
}
