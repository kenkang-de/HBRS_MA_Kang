#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include "../Paths.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <sstream>

class TestRunner {
private:
    std::string simulatorExecutable;
    std::string logDirectory;
    std::string testBaseName;
    
public:
    TestRunner(const std::string& executable = Paths::FromSampling::SIMULATOR_EXE, 
               const std::string& logDir = Paths::FromSampling::LOG_V1_DIR,
               const std::string& baseName = Paths::TEST_BASE_NAME);
    
    // Create the log directory if it doesn't exist
    bool createLogDirectory();
    
    // Run a single test and move the log file
    bool runSingleTest(int testNumber);
    
    // Run multiple tests
    bool runTestSuite(int startTest, int endTest);
    
    // Run exactly 10 tests (V1test1.txt to V1test10.txt)
    bool runStandardTestSuite();
    
    // Clean up old test files before running
    void cleanOldTests(int startTest, int endTest);
    
    // Check if simulator executable exists
    bool checkSimulatorExists();
    
    // Print status messages
    void printStatus(const std::string& message);
};

// Implementation (header-only)
inline TestRunner::TestRunner(const std::string& executable, 
                             const std::string& logDir,
                             const std::string& baseName) 
    : simulatorExecutable(executable), logDirectory(logDir), testBaseName(baseName) {
}

inline bool TestRunner::createLogDirectory() {
    try {
        std::filesystem::create_directories(logDirectory);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory " << logDirectory << ": " << e.what() << std::endl;
        return false;
    }
}

inline bool TestRunner::checkSimulatorExists() {
    return std::filesystem::exists(simulatorExecutable);
}

inline void TestRunner::printStatus(const std::string& message) {
    std::cout << "[TestRunner] " << message << std::endl;
}

inline bool TestRunner::runSingleTest(int testNumber) {
    printStatus("Running test " + std::to_string(testNumber) + "...");
    
    // Create target file path
    std::string targetFile = logDirectory + testBaseName + std::to_string(testNumber) + ".txt";
    
    // Run the simulator and redirect output to the target file
    std::string command = simulatorExecutable + " > \"" + targetFile + "\" 2>&1";
    int result = std::system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Error: Simulator execution failed with code " << result << std::endl;
        return false;
    }
    
    // Check if the file was created successfully
    if (!std::filesystem::exists(targetFile)) {
        std::cerr << "Error: Output file was not created: " << targetFile << std::endl;
        return false;
    }
    
    // Check if the file has content
    std::ifstream file(targetFile);
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            std::cerr << "Warning: Output file is empty: " << targetFile << std::endl;
        }
        file.close();
    }
    
    printStatus("Test " + std::to_string(testNumber) + " completed -> " + targetFile);
    return true;
}

inline bool TestRunner::runTestSuite(int startTest, int endTest) {
    if (!checkSimulatorExists()) {
        std::cerr << "Error: Simulator executable '" << simulatorExecutable << "' not found!" << std::endl;
        return false;
    }
    
    if (!createLogDirectory()) {
        return false;
    }
    
    printStatus("Starting test suite: " + testBaseName + std::to_string(startTest) + 
                " to " + testBaseName + std::to_string(endTest));
    
    int successCount = 0;
    int totalTests = endTest - startTest + 1;
    
    for (int i = startTest; i <= endTest; ++i) {
        if (runSingleTest(i)) {
            successCount++;
        } else {
            std::cerr << "Test " << i << " failed!" << std::endl;
        }
    }
    
    printStatus("Test suite completed: " + std::to_string(successCount) + "/" + 
                std::to_string(totalTests) + " tests successful");
    
    return successCount == totalTests;
}

inline bool TestRunner::runStandardTestSuite() {
    return runTestSuite(1, 10);
}

inline void TestRunner::cleanOldTests(int startTest, int endTest) {
    printStatus("Cleaning old test files...");
    
    for (int i = startTest; i <= endTest; ++i) {
        std::string testFile = logDirectory + testBaseName + std::to_string(i) + ".txt";
        if (std::filesystem::exists(testFile)) {
            try {
                std::filesystem::remove(testFile);
                printStatus("Removed old file: " + testFile);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not remove " << testFile << ": " << e.what() << std::endl;
            }
        }
    }
}

#endif // TESTRUNNER_H
