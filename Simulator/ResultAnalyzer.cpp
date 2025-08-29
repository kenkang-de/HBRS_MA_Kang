#include "ResultAnalyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <iomanip>

ResultAnalyzer::ResultAnalyzer(const std::string& logDir) : logDirectory(logDir) {
    // Constructor
}

ResultAnalyzer::~ResultAnalyzer() {
    // Destructor
}

int ResultAnalyzer::extractDamageFromLine(const std::string& line) {
    std::regex damageRegex(R"(took damage: (\d+))");
    std::smatch match;
    if (std::regex_search(line, match, damageRegex)) {
        return std::stoi(match[1].str());
    }
    return 0;
}

std::string ResultAnalyzer::extractWinnerFromLine(const std::string& line) {
    std::regex winnerRegex(R"(Battle Over! (.*?) Team wins!)");
    std::smatch match;
    if (std::regex_search(line, match, winnerRegex)) {
        return match[1].str();
    }
    return "Unknown";
}

int ResultAnalyzer::extractTickFromLine(const std::string& line) {
    std::regex tickRegex(R"(\[Tick (\d+)\])");
    std::smatch match;
    if (std::regex_search(line, match, tickRegex)) {
        return std::stoi(match[1].str());
    }
    return 0;
}

bool ResultAnalyzer::isRedUnit(const std::string& unit) {
    return unit.find("R") == 0 && unit.length() >= 2 && std::isdigit(unit[1]);
}

bool ResultAnalyzer::isBlueUnit(const std::string& unit) {
    return unit.find("B") == 0 && unit.length() >= 2 && std::isdigit(unit[1]);
}

void ResultAnalyzer::analyzeTest(const std::string& testFile) {
    std::string fullPath = logDirectory + testFile;
    std::cout << "Attempting to analyze: " << fullPath << std::endl;
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fullPath << std::endl;
        return;
    }
    
    std::cout << "Successfully opened: " << testFile << std::endl;
    
    BattleResult result;
    result.testName = testFile;
    
    std::string line;
    int maxTick = 0;
    int lineCount = 0;
    std::regex unitDamageRegex(R"(([RB]\d+) took damage: (\d+))");
    std::smatch match;
    
    while (std::getline(file, line)) {
        lineCount++;
        
        // Handle UTF-16 encoding by removing null bytes
        std::string cleanLine;
        for (size_t i = 0; i < line.length(); ++i) {
            if (line[i] != '\0') {
                cleanLine += line[i];
            }
        }
        
        // Skip BOM if present at the beginning of first line
        if (lineCount == 1 && cleanLine.length() >= 2 && 
            (unsigned char)cleanLine[0] == 0xFF && (unsigned char)cleanLine[1] == 0xFE) {
            cleanLine = cleanLine.substr(2);
        }
        
        line = cleanLine;  // Use the cleaned line for processing
        // Debug: print lines that contain "took damage"
        if (line.find("took damage") != std::string::npos) {
            //std::cout << "Processing damage line: " << line << std::endl;
        }
        
        // Extract tick information
        int currentTick = extractTickFromLine(line);
        if (currentTick > maxTick) {
            maxTick = currentTick;
        }
        
        // Extract damage information
        if (std::regex_search(line, match, unitDamageRegex)) {
            std::string unit = match[1].str();
            int damage = std::stoi(match[2].str());
            
            //std::cout << "Found damage: " << unit << " took " << damage << " damage" << std::endl;
            
            if (isRedUnit(unit)) {
                result.redDamageTaken += damage;
            } else if (isBlueUnit(unit)) {
                result.blueDamageTaken += damage;
            }
        }
        
        // Extract winner information
        if (line.find("Battle Over!") != std::string::npos) {
            result.winner = extractWinnerFromLine(line);
            //std::cout << "Found winner: " << result.winner << std::endl;
        }
    }
    
    result.totalTicks = maxTick;
    
    // Calculate damage ratio (avoid division by zero)
    if (result.redDamageTaken > 0) {
        result.damageRatio = static_cast<double>(result.blueDamageTaken) / result.redDamageTaken;
    } else if (result.blueDamageTaken > 0) {
        result.damageRatio = 999.0; // Indicate overwhelming blue advantage
    } else {
        result.damageRatio = 0.0;
    }
    
    // Check for stalemate (no winner found or very high tick count)
    result.isStalemate = (result.winner == "Unknown" || maxTick > 1000);
    
    results.push_back(result);
    file.close();
}

void ResultAnalyzer::analyzeTestRange(const std::string& baseTestName, int startNum, int endNum) {
    for (int i = startNum; i <= endNum; ++i) {
        std::string testFile = baseTestName + std::to_string(i) + ".txt";
        analyzeTest(testFile);
    }
}

void ResultAnalyzer::analyzeAllTests() {
    // This would require directory listing functionality
    // For now, we'll analyze the common V1Test range
    analyzeTestRange("V1Test", 1, 12);
}

BattleResult ResultAnalyzer::getTestResult(const std::string& testName) const {
    for (const auto& result : results) {
        if (result.testName == testName) {
            return result;
        }
    }
    return BattleResult(); // Return empty result if not found
}

std::vector<BattleResult> ResultAnalyzer::getAllResults() const {
    return results;
}

double ResultAnalyzer::getAverageRedDamage() const {
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : results) {
        total += result.redDamageTaken;
    }
    return total / results.size();
}

double ResultAnalyzer::getAverageBlueDamage() const {
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : results) {
        total += result.blueDamageTaken;
    }
    return total / results.size();
}

double ResultAnalyzer::getAverageDamageRatio() const {
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    int validResults = 0;
    for (const auto& result : results) {
        if (!result.isStalemate && result.damageRatio < 999.0) {
            total += result.damageRatio;
            validResults++;
        }
    }
    return validResults > 0 ? total / validResults : 0.0;
}

int ResultAnalyzer::getRedWinCount() const {
    int count = 0;
    for (const auto& result : results) {
        if (result.winner == "Red") {
            count++;
        }
    }
    return count;
}

int ResultAnalyzer::getBlueWinCount() const {
    int count = 0;
    for (const auto& result : results) {
        if (result.winner == "Blue") {
            count++;
        }
    }
    return count;
}

int ResultAnalyzer::getStalemateCount() const {
    int count = 0;
    for (const auto& result : results) {
        if (result.isStalemate) {
            count++;
        }
    }
    return count;
}

void ResultAnalyzer::printTestResult(const std::string& testName) const {
    BattleResult result = getTestResult(testName);
    if (result.testName.empty()) {
        std::cout << "Test " << testName << " not found!" << std::endl;
        return;
    }
    
    std::cout << "=== " << result.testName << " ===" << std::endl;
    std::cout << "Red Damage Taken: " << result.redDamageTaken << std::endl;
    std::cout << "Blue Damage Taken: " << result.blueDamageTaken << std::endl;
    std::cout << "Winner: " << result.winner << std::endl;
    std::cout << "Damage Ratio (Blue:Red): " << std::fixed << std::setprecision(2) << result.damageRatio << ":1" << std::endl;
    std::cout << "Total Ticks: " << result.totalTicks << std::endl;
    std::cout << "Stalemate: " << (result.isStalemate ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

void ResultAnalyzer::printAllResults() const {
    for (const auto& result : results) {
        printTestResult(result.testName);
    }
}

void ResultAnalyzer::printStatistics() const {
    if (results.empty()) {
        std::cout << "No results to analyze!" << std::endl;
        return;
    }
    
    std::cout << "=== BATTLE STATISTICS ===" << std::endl;
    std::cout << "Total Tests: " << results.size() << std::endl;
    std::cout << "Red Wins: " << getRedWinCount() << std::endl;
    std::cout << "Blue Wins: " << getBlueWinCount() << std::endl;
    std::cout << "Stalemates: " << getStalemateCount() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Average Red Damage Taken: " << std::fixed << std::setprecision(1) << getAverageRedDamage() << std::endl;
    std::cout << "Average Blue Damage Taken: " << std::fixed << std::setprecision(1) << getAverageBlueDamage() << std::endl;
    std::cout << "Average Damage Ratio: " << std::fixed << std::setprecision(2) << getAverageDamageRatio() << ":1" << std::endl;
    std::cout << std::endl;
    
    // Win rate percentages
    double redWinRate = (static_cast<double>(getRedWinCount()) / results.size()) * 100.0;
    double blueWinRate = (static_cast<double>(getBlueWinCount()) / results.size()) * 100.0;
    double stalemateRate = (static_cast<double>(getStalemateCount()) / results.size()) * 100.0;
    
    std::cout << "Red Win Rate: " << std::fixed << std::setprecision(1) << redWinRate << "%" << std::endl;
    std::cout << "Blue Win Rate: " << std::fixed << std::setprecision(1) << blueWinRate << "%" << std::endl;
    std::cout << "Stalemate Rate: " << std::fixed << std::setprecision(1) << stalemateRate << "%" << std::endl;
}

void ResultAnalyzer::saveResultsToFile(const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file " << outputFile << std::endl;
        return;
    }
    
    file << "Test Name,Red Damage,Blue Damage,Winner,Damage Ratio,Total Ticks,Stalemate" << std::endl;
    
    for (const auto& result : results) {
        file << result.testName << ","
             << result.redDamageTaken << ","
             << result.blueDamageTaken << ","
             << result.winner << ","
             << std::fixed << std::setprecision(2) << result.damageRatio << ","
             << result.totalTicks << ","
             << (result.isStalemate ? "Yes" : "No") << std::endl;
    }
    
    file.close();
    std::cout << "Results saved to " << outputFile << std::endl;
}

void ResultAnalyzer::clearResults() {
    results.clear();
}

int ResultAnalyzer::getTestCount() const {
    return results.size();
}
