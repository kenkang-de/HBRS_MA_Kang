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
};

#endif


