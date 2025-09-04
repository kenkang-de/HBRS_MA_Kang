#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <algorithm>

class SimpleBatchRunner {
private:
    std::string batchDirectory;
    std::string simulatorDir;
    std::string logDirectory;
    int simulationsPerBatch;
    
public:
    SimpleBatchRunner(
        const std::string& batchDir = "Batches/",
        const std::string& simDir = "../Simulator/",
        const std::string& logDir = "../Log/V1/",
        int simPerBatch = 10
    ) : batchDirectory(batchDir), simulatorDir(simDir), logDirectory(logDir), simulationsPerBatch(simPerBatch) {
        // Create log directory if it doesn't exist
        std::filesystem::create_directories(logDirectory);
    }
    
    bool runSingleBatchTest(const std::string& batchFile) {
        // Copy batch file to simulator directory as battle_configs.csv
        std::string sourcePath = batchDirectory + batchFile;
        std::string targetPath = simulatorDir + "battle_configs.csv";
        
        try {
            std::filesystem::copy_file(sourcePath, targetPath, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::exception& e) {
            std::cerr << "[SimpleBatchRunner] Failed to copy batch file: " << e.what() << std::endl;
            return false;
        }
        
        // Extract batch number for unique log naming
        std::string batchNumber = extractBatchNumber(batchFile);
        
        // Clear Simulator's Log/V1 directory before running
        clearSimulatorLogDirectory();
        
        // Change to simulator directory and run simulation
        std::string command = "cd \"" + simulatorDir + "\" && "
                            + ".\\Simulator.exe " + std::to_string(simulationsPerBatch);
        
        std::cout << "Running command: " << command << std::endl;
        
        int result = std::system(command.c_str());
        
        if (result == 0) {
            // Move generated log files to our batch-specific location
            moveLogFiles(batchNumber);
            return true;
        }
        
        return false;
    }
    
private:    
    std::string extractBatchNumber(const std::string& filename) {
        // Extract number from "batch_X_config.csv"
        size_t start = filename.find("batch_") + 6;
        size_t end = filename.find("_config.csv");
        if (start != std::string::npos && end != std::string::npos) {
            return filename.substr(start, end - start);
        }
        return "unknown";
    }
    
    void clearSimulatorLogDirectory() {
        // Clear all .txt files from Simulator's Log/V1 directory before running
        std::string sourceLogDir = simulatorDir + "Log/V1/";
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(sourceLogDir)) {
                if (entry.path().extension() == ".txt") {
                    std::filesystem::remove(entry.path());
                    std::cout << "[SimpleBatchRunner] Removed old log file: " << entry.path().filename().string() << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "[SimpleBatchRunner] Warning: Could not clear log directory: " << e.what() << std::endl;
        }
    }
    
    void moveLogFiles(const std::string& batchNumber) {
        // Move log files from Simulator/Log/V1/ to our target directory with batch prefix
        std::string sourceLogDir = simulatorDir + "Log/V1/";
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(sourceLogDir)) {
                if (entry.path().extension() == ".txt") {
                    std::string filename = entry.path().filename().string();
                    std::string newFilename = "Batch" + batchNumber + "_" + filename;
                    std::string targetPath = logDirectory + newFilename;
                    
                    std::filesystem::copy_file(entry.path(), targetPath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "[SimpleBatchRunner] Moved: " << filename << " -> " << newFilename << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "[SimpleBatchRunner] Warning: Could not move log files: " << e.what() << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Single Batch Test ===" << std::endl;
    
    SimpleBatchRunner runner("Batches/", "../Simulator/", "../Log/V1/", 10);
    
    if (runner.runSingleBatchTest("batch_1_config.csv")) {
        std::cout << "=== Single batch test completed ===" << std::endl;
        return 0;
    } else {
        std::cerr << "=== Single batch test failed ===" << std::endl;
        return 1;
    }
}
