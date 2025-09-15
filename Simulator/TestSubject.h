#ifndef TESTSUB_H
#define TESTSUB_H

#include <string>

class TestSubject {
protected:
    std::string id;

public:
    // Constructor
    TestSubject(const std::string& id = "") : id(id), WinRate(0.0f), TotalWin(0), TotalLost(0), TotalDraw(0), UsageCount(0) {}
    
    // Virtual destructor for proper inheritance
    virtual ~TestSubject() = default;
    
    // Getters
    const std::string& GetID() const { return id; }
    
    // Statistics
    float WinRate;
    int TotalWin;
    int TotalLost;
    int TotalDraw;
    int UsageCount;

    //Sampling
    int SampleUsage;
    std::vector<int> SamplePartnerIndexList;
    
    // Statistics methods
    void recordWin() {
        TotalWin++;
        updateWinRate();
    }
    
    void recordLoss() {
        TotalLost++;
        updateWinRate();
    }
    
    void recordDraw() {
        TotalDraw++;
        updateWinRate();
    }
    
    void incrementUsage() {
        UsageCount++;
    }
    
    void resetResults() {
        WinRate = 0.0f;
        TotalWin = 0;
        TotalLost = 0;
        TotalDraw = 0;
        UsageCount = 0;
    }

private:
    void updateWinRate() {
        int totalBattles = TotalWin + TotalLost + TotalDraw;
        if (totalBattles > 0) {
            WinRate = static_cast<float>(TotalWin) / totalBattles;
        } else {
            WinRate = 0.0f;
        }
    }
};

#endif


