@echo off
echo Building Simulator...

set SOURCES=Simulator.cpp Stat.cpp Unit.cpp Team.cpp UnitGenerator.cpp EquipmentManager.cpp TurnManager.cpp BattleManager.cpp BattleAction.cpp BattleActionLoader.cpp BattleActionParser.cpp TargetManager.cpp ActionLibrary.cpp Battlefield.cpp BoonAction.cpp TempBoonAction.cpp ../Analysis/ElementAnalysis.cpp
set INCLUDES=-I yaml-cpp/include
set LIBS=-L yaml-cpp/mingw-build -lyaml-cpp
set FLAGS=-g -std=c++17 -DYAML_CPP_STATIC_DEFINE

g++ %FLAGS% %SOURCES% %INCLUDES% %LIBS% -o Simulator.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Simulator.exe created.
) else (
    echo Build failed!
    exit /b 1
)
