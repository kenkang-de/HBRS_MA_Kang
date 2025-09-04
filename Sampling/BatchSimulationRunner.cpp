#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>

class BatchSimulationRunner {
private:
    std::string batchDirectory;
    std::string simulatorPath;
    std::string logDirectory;
    int simulationsPerBatch;
    
public:
    BatchSimulationRunner(
        const std::string& batchDir = "../Sampling/Batches/",
        const std::string& simPath = "../Simulator/Simulator.exe",
        const std::string& logDir = "../Log/V1/",
        int simPerBatch = 10
    ) : batchDirectory(batchDir), simulatorPath(simPath), logDirectory(logDir), simulationsPerBatch(simPerBatch) {
        // Create log directory if it doesn't exist
        std::filesystem::create_directories(logDirectory);
    }
    
    bool runAllBatchSimulations() {
        std::cout << "[BatchRunner] Starting batch simulations..." << std::endl;
        
        // Find all batch files
        std::vector<std::string> batchFiles;
        for (const auto& entry : std::filesystem::directory_iterator(batchDirectory)) {
            if (entry.path().extension() == ".csv" && 
                entry.path().filename().string().find("batch_") == 0) {
                batchFiles.push_back(entry.path().filename().string());
            }
        }
        
        std::sort(batchFiles.begin(), batchFiles.end());
        
        std::cout << "[BatchRunner] Found " << batchFiles.size() << " batch files" << std::endl;
        
        int totalMatches = 0;
        int successfulBatches = 0;
        
        for (const auto& batchFile : batchFiles) {
            std::cout << "[BatchRunner] Processing " << batchFile << "..." << std::endl;
            
            if (runBatchSimulation(batchFile)) {
                successfulBatches++;
                totalMatches += countMatchesInBatch(batchFile);
            } else {
                std::cerr << "[BatchRunner] Failed to process " << batchFile << std::endl;
            }
        }
        
        std::cout << "[BatchRunner] Completed " << successfulBatches << "/" << batchFiles.size() 
                  << " batches (" << totalMatches << " total matches)" << std::endl;
        
        return successfulBatches > 0;
    }
    
private:
    bool runBatchSimulation(const std::string& batchFile) {
        std::string batchPath = batchDirectory + batchFile;
        
        // Extract batch number from filename (e.g., "batch_1_config.csv" -> "1")
        std::string batchNumber = extractBatchNumber(batchFile);
        std::string logFile = logDirectory + "BatchResults_" + batchNumber + ".txt";
        
        // Build command to run simulator with batch configuration
        std::string command = "cd \"" + std::filesystem::current_path().string() + "\" && "
                            + "\"" + simulatorPath + "\" "
                            + "\"" + batchPath + "\" "
                            + std::to_string(simulationsPerBatch) + " "
                            + "\"" + logFile + "\"";
        
        int result = std::system(command.c_str());
        return result == 0;
    }
    
    std::string extractBatchNumber(const std::string& filename) {
        // Extract number from "batch_X_config.csv"
        size_t start = filename.find("batch_") + 6;
        size_t end = filename.find("_config.csv");
        if (start != std::string::npos && end != std::string::npos) {
            return filename.substr(start, end - start);
        }
        return "unknown";
    }
    
    int countMatchesInBatch(const std::string& batchFile) {
        std::ifstream file(batchDirectory + batchFile);
        if (!file.is_open()) return 0;
        
        int count = 0;
        std::string line;
        std::getline(file, line); // Skip header
        
        while (std::getline(file, line)) {
            if (!line.empty()) count++;
        }
        
        return count;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== Batch Simulation Runner ===" << std::endl;
    
    int simulationsPerBatch = 10;
    if (argc > 1) {
        simulationsPerBatch = std::stoi(argv[1]);
    }
    
    BatchSimulationRunner runner("../Sampling/Batches/", "../Simulator/Simulator.exe", "../Log/V1/", simulationsPerBatch);
    
    if (runner.runAllBatchSimulations()) {
        std::cout << "=== All batch simulations completed ===" << std::endl;
        std::cout << "Results available in: ../Log/V1/" << std::endl;
        return 0;
    } else {
        std::cerr << "=== Batch simulation failed ===" << std::endl;
        return 1;
    }
}
