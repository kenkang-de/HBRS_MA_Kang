#include "TestRunner.h"
#include <iostream>

int main() {
    std::cout << "=== Single Test Runner ===" << std::endl;
    
    // Create test runner
    TestRunner runner;
    
    // Run just one test to verify it works
    if (runner.runSingleTest(1)) {
        std::cout << "Test completed successfully!" << std::endl;
    } else {
        std::cout << "Test failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
