#include "LogSystem.h"
#include <iostream>

// Define static members
std::ofstream LogSystem::logFile;
bool LogSystem::isLogging = false;

bool LogSystem::StartLogging(const std::string &logFileName) {
    if (isLogging) {
        StopLogging();
    }

    logFile.open(logFileName);
    if (logFile.is_open()) {
        isLogging = true;
        logFile.flush();
        return true;
    }

    std::cerr << "Failed to open log file: " << logFileName << std::endl;
    return false;
}

void LogSystem::StopLogging() {
    if (isLogging && logFile.is_open()) {
        logFile.flush();
        logFile.close();
    }
    isLogging = false;
}

void LogSystem::Log(const std::string &message) {
    if (isLogging && logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush();
    }
}

std::string LogSystem::GetUnitListName(std::vector<Unit *> unitList) {
    std::string unitIDs = "";
    for (Unit *unit : unitList)
        unitIDs += unit->Name + ", ";
    return unitIDs;
}

void LogSystem::LogUnitListStats(std::vector<Unit *> unitList) {
    for (int i = 0; i < unitList.size(); i++) {
        Unit *unit = unitList[i];
        LogSystem::LogStream("Unit ", i, ": ", unit->GetName(), " Attack:", unit->GetTotalStat().GetAttack(),
                             " Defense:", unit->GetTotalStat().GetDefense(), " HP:", unit->GetTotalStat().GetHP(),
                             " Speed:", unit->GetTotalStat().GetSpeed(), " Threat:", unit->GetTotalStat().GetThreat(),
                             " Weapon: [", unit->GetWeapon()->GetID(), "]", "(", unit->GetWeapon()->GetAction().GetID(),
                             ")", " Armor: [", unit->GetArmor()->GetID(), "]");
    }
    LogSystem::LogStream("==========================");
}

void LogSystem::LogSurvivedUnit(std::vector<Unit *> unitList) {
    std::string unitIDs = "";
    for (Unit *unit : unitList) {
        if (unit->IsAlive())
            unitIDs += unit->Name + ", ";
    }
    LogSystem::LogStream("Survived Unit: ", unitIDs);
}
