@echo off

set TARGET=RunTests.exe

REM Check if executable exists and is newer than source files
if exist %TARGET% (
    echo Checking if rebuild is needed...
    
    REM Check RunTests.cpp
    for %%i in (RunTests.cpp) do set SOURCE_TIME=%%~ti
    for %%i in (%TARGET%) do set TARGET_TIME=%%~ti
    
    REM Simple string comparison (not perfect but good enough)
    if "%SOURCE_TIME%" GTR "%TARGET_TIME%" goto BUILD
    
    echo %TARGET% is up to date.
    goto END
)

:BUILD
echo Building %TARGET%...

g++ -g -std=c++17 -DYAML_CPP_STATIC_DEFINE ^
    -I../Simulator/yaml-cpp/include ^
    -o %TARGET% ^
    RunTests.cpp ^
    ../Simulator/Stat.cpp ^
    ../Simulator/Unit.cpp ^
    ../Simulator/Team.cpp ^
    ../Simulator/UnitGenerator.cpp ^
    ../Simulator/EquipmentManager.cpp ^
    ../Simulator/TurnManager.cpp ^
    ../Simulator/BattleManager.cpp ^
    ../Simulator/BattleAction.cpp ^
    ../Simulator/BattleActionLoader.cpp ^
    ../Simulator/BattleActionParser.cpp ^
    ../Simulator/TargetManager.cpp ^
    ../Simulator/ActionLibrary.cpp ^
    ../Simulator/Battlefield.cpp ^
    ../Simulator/BoonAction.cpp ^
    ../Simulator/TempBoonAction.cpp ^
    ../Analysis/ElementAnalysis.cpp ^
    -L../Simulator/yaml-cpp/mingw-build -lyaml-cpp

if %ERRORLEVEL%==0 (
    echo Build complete: %TARGET%
) else (
    echo Build failed!
    exit /b 1
)

:END
