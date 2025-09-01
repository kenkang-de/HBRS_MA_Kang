@echo off
setlocal enabledelayedexpansion

set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -O2
set INCLUDES=-I. -I../Simulator -I../Analysis -I../Simulator/yaml-cpp/include
set LIBS=-L../Simulator/yaml-cpp/build -lyaml-cpp
set TARGET=RunTests.exe

echo === Fast Incremental Build ===

REM Check if we need to rebuild
set NEED_REBUILD=0
if not exist %TARGET% set NEED_REBUILD=1

REM Quick check - if any .cpp is newer than exe, rebuild
if %NEED_REBUILD%==0 (
    for %%f in (RunTests.cpp ../Simulator/*.cpp ../Analysis/*.cpp) do (
        if "%%f" NEQ "" (
            for /f %%i in ('forfiles /m %TARGET% /c "cmd /c echo @fdate @ftime"') do set TARGET_TIME=%%i
            for /f %%j in ('forfiles /m "%%f" /c "cmd /c echo @fdate @ftime"') do (
                if "%%j" GTR "!TARGET_TIME!" set NEED_REBUILD=1
            )
        )
    )
)

if %NEED_REBUILD%==1 (
    echo Building %TARGET%...
    %CXX% %CXXFLAGS% %INCLUDES% -o %TARGET% ^
        RunTests.cpp ^
        ../Simulator/Simulator.cpp ^
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
        ../Simulator/BattleActionLoader.cpp ^
        ../Simulator/BattleActionParser.cpp ^
        ../Simulator/ActionLibrary.cpp ^
        ../Simulator/TargetManager.cpp ^
        ../Analysis/ElementAnalysis.cpp ^
        ../Analysis/TestSubject.cpp ^
        %LIBS%
    
    if %ERRORLEVEL%==0 (
        echo Build complete!
    ) else (
        echo Build failed!
        exit /b 1
    )
) else (
    echo %TARGET% is up to date, skipping build.
)
