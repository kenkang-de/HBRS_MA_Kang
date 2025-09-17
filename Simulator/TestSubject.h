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
    
    // Statistics - make them mutable so they can be modified from const methods
    mutable float WinRate;
    mutable int TotalWin;
    mutable int TotalLost;
    mutable int TotalDraw;
    mutable int UsageCount;

    //Sampling
    int SampleUsage;
    // std::vector<int> SamplePartnerIndexList;
    
    // Statistics methods
     void recordWin() const {
        TotalWin++;
        updateWinRate();
    }
    
     void recordLoss() const {
        TotalLost++;
        updateWinRate();
    }
    
     void recordDraw() const {
        TotalDraw++;
        updateWinRate();
    }
    
     void incrementUsage() const {
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
    void updateWinRate() const {
        int totalBattles = TotalWin + TotalLost + TotalDraw;
        if (totalBattles > 0) {
            WinRate = static_cast<float>(TotalWin) / totalBattles;
        } else {
            WinRate = 0.0f;
        }
    }
};

#endif


