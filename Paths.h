#ifndef PATHS_H
#define PATHS_H

#include <string>

// Central path configuration for the MA Project
namespace Paths {
// Base directories (relative to project root)
const std::string SIMULATOR_DIR = "Simulator/";
const std::string DATA_DIR = "Data/";
const std::string ANALYSIS_DIR = "Analysis/";
const std::string SAMPLING_DIR = "Sampling/";
const std::string ELEMENT_DIR = "Element/";
const std::string LOG_DIR = "Log/";

// Data files (CSV, YAML, etc.)
const std::string ARMOR_DATA_CSV = "Armor_v1.csv";
const std::string WEAPON_DATA_CSV = "Weapon_v1.csv";
const std::string BATTLE_ACTIONS_YAML = "BattleActions.yaml";
const std::string MASTER_CONFIG = "master_config.txt";

// Specific log directories
const std::string LOG_V1_DIR = LOG_DIR + "V1/";
const std::string LOG_TEST_DIR = LOG_DIR + "Test/";
const std::string LOG_BALANCING_V1_DIR = LOG_DIR + "Balancing/V1/";

// Executables (relative to project root)
const std::string SIMULATOR_EXE = SIMULATOR_DIR + "Simulator.exe";

// Relative paths for different components
namespace FromSampling {
const std::string SIMULATOR_EXE = "../" + Paths::SIMULATOR_EXE;
const std::string LOG_V1_DIR = "../" + Paths::LOG_V1_DIR;
const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
} // namespace FromSampling

namespace FromAnalysis {
const std::string LOG_V1_DIR = "../" + Paths::LOG_V1_DIR;
const std::string LOG_BALANCING_V1_DIR = Paths::LOG_BALANCING_V1_DIR;
const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
} // namespace FromAnalysis

namespace FromSimulator {
const std::string ELEMENT_DIR = "../" + Paths::ELEMENT_DIR;
const std::string BATTLELOG_DIR = "../" + Paths::LOG_V1_DIR;
} // namespace FromSimulator

namespace FromElement {
const std::string DATA_DIR = "../" + Paths::DATA_DIR;
const std::string SIMULATOR_DIR = "../" + Paths::SIMULATOR_DIR;
const std::string SAMPLING_DIR = "../" + Paths::SAMPLING_DIR;
} // namespace FromElement

// Output files
const std::string BATTLE_ANALYSIS_CSV = "battle_analysis.csv";

// Test configuration
const std::string TEST_BASE_NAME = "V1test";

} // namespace Paths

#endif // PATHS_H
