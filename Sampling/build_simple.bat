@echo off
echo Building minimal RunTests without YAML dependencies...

g++ -std=c++17 -Wall -O2 ^
    -I. -I../Simulator -I../Analysis ^
    -o RunTests_simple.exe ^
    RunTests.cpp ^
    ../Simulator/BattleManager.cpp ^
    ../Simulator/TurnManager.cpp ^
    ../Simulator/Battlefield.cpp ^
    ../Simulator/Team.cpp ^
    ../Simulator/Unit.cpp ^
    ../Simulator/UnitGenerator.cpp ^
    ../Simulator/Stat.cpp ^
    ../Simulator/EquipmentManager.cpp ^
    ../Simulator/BattleAction.cpp ^
    ../Simulator/BoonAction.cpp ^
    ../Simulator/TempBoonAction.cpp ^
    ../Simulator/TargetManager.cpp ^
    ../Analysis/ElementAnalysis.cpp

if %ERRORLEVEL%==0 (
    echo Build successful: RunTests_simple.exe
) else (
    echo Build failed - trying without BattleActionLoader and ActionLibrary...
    
    g++ -std=c++17 -Wall -O2 ^
        -I. -I../Simulator -I../Analysis ^
        -o RunTests_minimal.exe ^
        RunTests.cpp ^
        ../Simulator/BattleManager.cpp ^
        ../Simulator/TurnManager.cpp ^
        ../Simulator/Battlefield.cpp ^
        ../Simulator/Team.cpp ^
        ../Simulator/Unit.cpp ^
        ../Simulator/UnitGenerator.cpp ^
        ../Simulator/Stat.cpp ^
        ../Simulator/EquipmentManager.cpp ^
        ../Analysis/ElementAnalysis.cpp
        
    if %ERRORLEVEL%==0 (
        echo Minimal build successful: RunTests_minimal.exe
    ) else (
        echo All builds failed!
    )
)
