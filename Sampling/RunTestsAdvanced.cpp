#include "TestRunner.h"
#include <iostream>
#include <string>

void printUsage() {
    std::cout << "Usage: RunTestsAdvanced [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -n <count>     Number of tests to run (default: 10)" << std::endl;
    std::cout << "  -s <start>     Starting test number (default: 1)" << std::endl;
    std::cout << "  -d <dir>       Log directory (default: Log/V1/)" << std::endl;
    std::cout << "  -b <basename>  Test base name (default: V1test)" << std::endl;
    std::cout << "  -e <exe>       Simulator executable (default: Simulator.exe)" << std::endl;
    std::cout << "  -h             Show this help" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default settings
    int testCount = 10;
    int startNumber = 1;
    std::string logDir = "Log/V1/";
    std::string baseName = "V1test";
    std::string executable = "Simulator.exe";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-n" && i + 1 < argc) {
            testCount = std::stoi(argv[++i]);
        } else if (arg == "-s" && i + 1 < argc) {
            startNumber = std::stoi(argv[++i]);
        } else if (arg == "-d" && i + 1 < argc) {
            logDir = argv[++i];
            if (logDir.back() != '/') logDir += "/";
        } else if (arg == "-b" && i + 1 < argc) {
            baseName = argv[++i];
        } else if (arg == "-e" && i + 1 < argc) {
            executable = argv[++i];
        }
    }
    
    int endNumber = startNumber + testCount - 1;
    
    std::cout << "=== Advanced Simulator Test Suite Runner ===" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Executable: " << executable << std::endl;
    std::cout << "Log Directory: " << logDir << std::endl;
    std::cout << "Base Name: " << baseName << std::endl;
    std::cout << "Test Range: " << startNumber << " to " << endNumber << std::endl;
    std::cout << "============================================" << std::endl << std::endl;
    
    // Create test runner with custom settings
    TestRunner runner(executable, logDir, baseName);
    
    // Clean old test files
    runner.cleanOldTests(startNumber, endNumber);
    
    // Run the test suite
    if (runner.runTestSuite(startNumber, endNumber)) {
        std::cout << std::endl << "All tests completed successfully!" << std::endl;
        std::cout << "Results saved in " << logDir << " directory" << std::endl;
    } else {
        std::cout << std::endl << "Some tests failed. Check the output above for details." << std::endl;
        return 1;
    }
    
    return 0;
}
