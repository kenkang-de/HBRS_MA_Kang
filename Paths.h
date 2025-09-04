#ifndef PATHS_H
#define PATHS_H

#include <string>

// Central path configuration for the MA Project
namespace Paths {
    // Base directories (relative to project root)
    const std::string SIMULATOR_DIR = "Simulator/";
    const std::string ANALYSIS_DIR = "Analysis/";
    const std::string SAMPLING_DIR = "Sampling/";
    const std::string ELEMENT_DIR = "Element/";
    const std::string LOG_DIR = "Log/";
    const std::string DATA_DIR = "Simulator/Data/";
    
    // Specific log directories
    const std::string LOG_V1_DIR = LOG_DIR + "V1/";
    const std::string LOG_TEST_DIR = LOG_DIR + "Test/";
    
    // Executables (relative to project root)
    const std::string SIMULATOR_EXE = SIMULATOR_DIR + "Simulator.exe";
    
    // Relative paths for different components
    namespace FromSampling {
        const std::string SIMULATOR_EXE = "../" + Paths::SIMULATOR_EXE;
        const std::string LOG_V1_DIR = "../" + Paths::LOG_V1_DIR;
        const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
    }
    
    namespace FromAnalysis {
        const std::string LOG_V1_DIR = "../" + Paths::LOG_V1_DIR;
        const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
    }
    
    namespace FromSimulator {
        const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
    }
    
    namespace FromElement {
        const std::string DATA_DIR = "../" + Paths::DATA_DIR;
        const std::string SIMULATOR_DIR = "../" + Paths::SIMULATOR_DIR;
        const std::string SAMPLING_DIR = "../" + Paths::SAMPLING_DIR;
    }
    
    // Output files
    const std::string BATTLE_ANALYSIS_CSV = "battle_analysis.csv";
    
    // Test configuration
    const std::string TEST_BASE_NAME = "V1test";
}

#endif // PATHS_H
