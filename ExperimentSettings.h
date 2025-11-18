#ifndef EXPERIMENTSETTINGS_H
#define EXPERIMENTSETTINGS_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class ExperimentSettings {
  public:
    // Default values
    static constexpr int Default_APPLIEDSTAT_RANGE = 10;
    static constexpr int Default_INDIVIDUALS_PER_GENERATION = 50;
    static constexpr int Default_MAXGENERATION = 50;
    static constexpr int Default_ELITES_PER_GENERATION = 1;
    static constexpr float Default_CROSSOVER_PROBABILITY = 0.9f;
    static constexpr float Default_MUTATION_PROBABILITY = 0.3f;
    static constexpr float Default_MUTATION_SIGMA = 1.2f;

    inline static int APPLIEDSTAT_RANGE = Default_APPLIEDSTAT_RANGE;
    inline static int INDIVIDUALS_PER_GENERATION = Default_INDIVIDUALS_PER_GENERATION;
    inline static int MAXGENERATION = Default_MAXGENERATION;
    inline static int ELITES_PER_GENERATION = Default_ELITES_PER_GENERATION;
    inline static float CROSSOVER_PROBABILITY = Default_CROSSOVER_PROBABILITY;
    inline static float MUTATION_PROBABILITY = Default_MUTATION_PROBABILITY;
    inline static float MUTATION_SIGMA = Default_MUTATION_SIGMA;

    void LoadFromFile(const std::string &filename) {
        APPLIEDSTAT_RANGE = Default_APPLIEDSTAT_RANGE;
        INDIVIDUALS_PER_GENERATION = Default_INDIVIDUALS_PER_GENERATION;
        MAXGENERATION = Default_MAXGENERATION;
        ELITES_PER_GENERATION = Default_ELITES_PER_GENERATION;
        CROSSOVER_PROBABILITY = Default_CROSSOVER_PROBABILITY;
        MUTATION_PROBABILITY = Default_MUTATION_PROBABILITY;
        MUTATION_SIGMA = Default_MUTATION_SIGMA;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << filename << ". Using default values." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and commentsI mean
            if (line.empty() || line[0] == '#')
                continue;

            size_t equalPos = line.find('=');
            if (equalPos == std::string::npos) {
                // No '=' found, skip this line
                continue;
            }

            // Parse key=value pairs
            if (line.find("APPLIEDSTAT_RANGE=") == 0) {
                APPLIEDSTAT_RANGE = std::stoi(line.substr(18));
            } else if (line.find("INDIVIDUALS_PER_GENERATION=") == 0) {
                INDIVIDUALS_PER_GENERATION = std::stoi(line.substr(27));
            } else if (line.find("MAXGENERATION=") == 0) {
                MAXGENERATION = std::stoi(line.substr(14));
            } else if (line.find("ELITES_PER_GENERATION=") == 0) {
                ELITES_PER_GENERATION = std::stoi(line.substr(22));
            } else if (line.find("CROSSOVER_PROBABILITY=") == 0) {
                CROSSOVER_PROBABILITY = std::stof(line.substr(22));
            } else if (line.find("MUTATION_PROBABILITY=") == 0) {
                MUTATION_PROBABILITY = std::stof(line.substr(21));
            } else if (line.find("MUTATION_SIGMA=") == 0) {
                MUTATION_SIGMA = std::stof(line.substr(15));
            }
        }

        file.close();
        std::cout << "Loaded settings from: " << filename << std::endl;
    }
    std::vector<std::string> GetExperimentFiles(const std::string &directory) {
        std::vector<std::string> files;

        try {
            for (const auto &entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    files.push_back(entry.path().string());
                }
            }
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Error reading directory " << directory << ": " << e.what() << std::endl;
        }

        return files;
    }
};

#endif