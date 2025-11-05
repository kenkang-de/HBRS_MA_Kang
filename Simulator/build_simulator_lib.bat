@echo off
setlocal

REM Check if debug mode is requested
set DEBUG_MODE=0
if /i "%1"=="debug" set DEBUG_MODE=1

if %DEBUG_MODE%==1 (
    echo Building Simulator library in DEBUG mode...
    set COMPILE_FLAGS=-std=c++17 -g -O0 -DDEBUG
) else (
    echo Building Simulator library in RELEASE mode...
    set COMPILE_FLAGS=-std=c++17 -O2 -DNDEBUG
)

cd Simulator

g++ %COMPILE_FLAGS% -I. -I.. -Iyaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleAction.cpp -o BattleAction.o
g++ %COMPILE_FLAGS% -I. -I.. -c Unit.cpp -o Unit.o
g++ %COMPILE_FLAGS% -I. -I.. -c Battlefield.cpp -o Battlefield.o
g++ %COMPILE_FLAGS% -I. -I.. -c BattleManager.cpp -o BattleManager.o
g++ %COMPILE_FLAGS% -I. -I.. -c TargetManager.cpp -o TargetManager.o
g++ %COMPILE_FLAGS% -I. -I.. -c TurnManager.cpp -o TurnManager.o
g++ %COMPILE_FLAGS% -I. -I.. -c Team.cpp -o Team.o
g++ %COMPILE_FLAGS% -I. -I.. -c Simulator.cpp -o Simulator.o
g++ %COMPILE_FLAGS% -I. -I.. -c BoonAction.cpp -o BoonAction.o
g++ %COMPILE_FLAGS% -I. -I.. -c TempBoonAction.cpp -o TempBoonAction.o
g++ %COMPILE_FLAGS% -I. -I.. -c GlobalAction.cpp -o GlobalAction.o
g++ %COMPILE_FLAGS% -I. -I.. -c ActionLibrary.cpp -o ActionLibrary.o
g++ %COMPILE_FLAGS% -I. -I.. -c CounterRule.cpp -o CounterRule.o
g++ %COMPILE_FLAGS% -I. -I.. -c SynergyRule.cpp -o SynergyRule.o
g++ %COMPILE_FLAGS% -I. -I.. -c DelayRule.cpp -o DelayRule.o

ar rcs libsimulator.a BattleAction.o Unit.o Battlefield.o BattleManager.o Team.o TargetManager.o TurnManager.o Simulator.o BoonAction.o TempBoonAction.o GlobalAction.o ActionLibrary.o CounterRule.o SynergyRule.o DelayRule.o
cd..

if %DEBUG_MODE%==1 (
    echo Simulator library built successfully with DEBUG symbols!
) else (
    echo Simulator library built successfully in RELEASE mode!
)