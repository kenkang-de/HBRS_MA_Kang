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

    // rmse ranges from 0.0 (perfectly 0.5) to 0.5 (all 0 or all 1)
    // Normalize so that 0.0 -> 1.0, and 0.5 -> 0.0
    float normalized = 1.0f - (rmse / 0.5f);

    // Clamp and round
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    return std::round(normalized * 100.0f) / 100.0f;
}