#ifndef RESULT_ANALYZER_H
#define RESULT_ANALYZER_H

#include <string>
#include <vector>
#include <map>

struct BattleResult {
    std::string testName;
    int redDamageTaken;
    int blueDamageTaken;
    std::string winner;
    double damageRatio; // blue:red ratio
    int totalTicks;
    bool isStalemate;
    
    BattleResult() : redDamageTaken(0), blueDamageTaken(0), winner("Unknown"), 
                     damageRatio(0.0), totalTicks(0), isStalemate(false) {}
};

class ResultAnalyzer {
private:
    std::vector<BattleResult> results;
    std::string logDirectory;
    
    // Helper methods
    int extractDamageFromLine(const std::string& line);
    std::string extractWinnerFromLine(const std::string& line);
    int extractTickFromLine(const std::string& line);
    bool isRedUnit(const std::string& unit);
    bool isBlueUnit(const std::string& unit);
    
public:
    ResultAnalyzer(const std::string& logDir = "Log/Test/");
    ~ResultAnalyzer();
    
    // Main analysis methods
    void analyzeTest(const std::string& testFile);
    void analyzeTestRange(const std::string& baseTestName, int startNum, int endNum);
    void analyzeAllTests();
    
    // Result retrieval
    BattleResult getTestResult(const std::string& testName) const;
    std::vector<BattleResult> getAllResults() const;
    
    // Statistical analysis
    double getAverageRedDamage() const;
    double getAverageBlueDamage() const;
    double getAverageDamageRatio() const;
    int getRedWinCount() const;
    int getBlueWinCount() const;
    int getStalemateCount() const;
    
    // Output methods
    void printTestResult(const std::string& testName) const;
    void printAllResults() const;
    void printStatistics() const;
    void saveResultsToFile(const std::string& outputFile) const;
    
    // Utility methods
    void clearResults();
    int getTestCount() const;
};

#endif // RESULT_ANALYZER_H
