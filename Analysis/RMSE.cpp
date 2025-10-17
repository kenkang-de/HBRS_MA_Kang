 #include "RMSE.h"
 
float RMSE::Calculate(std::vector<TestSubject*> testSubjects)
{
    float sumSquaredErrors = 0.0f;
    
    for (TestSubject* subject : testSubjects) {
            float error = subject->WinRate - TARGET_WINRATE;
            sumSquaredErrors += error * error;
    }
    
    float meanSquaredError = sumSquaredErrors / testSubjects.size();
    
    float rmse = std::sqrt(meanSquaredError);
    
    // Round to 2 decimal places
    return std::round(rmse * 100.0f) / 100.0f;
}