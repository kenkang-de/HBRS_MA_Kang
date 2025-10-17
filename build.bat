@echo off
setlocal

REM Check if debug mode is requested
set DEBUG_MODE=0
if /i "%1"=="debug" set DEBUG_MODE=1

if %DEBUG_MODE%==1 (
    echo Building in DEBUG mode...
    set COMPILE_FLAGS=-std=c++17 -g -O0 -DDEBUG
    set LINK_FLAGS=-std=c++17 -g -O0
) else (
    echo Building in RELEASE mode...
    set COMPILE_FLAGS=-std=c++17 -O2 -DNDEBUG
    set LINK_FLAGS=-std=c++17 -O2
)

REM Always build/rebuild Simulator library to ensure it's up to date
call Simulator\build_simulator_lib.bat %1
if %ERRORLEVEL% NEQ 0 (
    echo Failed to build Simulator library
    exit /b 1
)

cd Log
echo Compiling LogSystem.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -c LogSystem.cpp -o LogSystem.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile LogSystem.cpp
    exit /b 1
)
cd..

REM Build Element components first
cd Element
echo Compiling BattleActionLoader.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleActionLoader.cpp -o BattleActionLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleActionLoader.cpp
    exit /b 1
)

echo Compiling Stat.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c Stat.cpp -o Stat.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile Stat.cpp
    exit /b 1
)

echo Compiling ElementList.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c ElementList.cpp -o ElementList.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile ElementList.cpp
    exit /b 1
)

echo Compiling BattleActionParser.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -c BattleActionParser.cpp -o BattleActionParser.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleActionParser.cpp
    exit /b 1
)

echo Compiling EquipmentLoader.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -c EquipmentLoader.cpp -o EquipmentLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile EquipmentLoader.cpp
    exit /b 1
)

echo Compiling UnitGenerator.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -c UnitGenerator.cpp -o UnitGenerator.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile UnitGenerator.cpp
    exit /b 1
)

echo Compiling CounterTypeInitializer.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Element -c CounterTypeInitializer.cpp -o CounterTypeInitializer.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile CounterTypeInitializer.cpp
    exit /b 1
)

echo Compiling SynergyComponentInitializer.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Element -c SynergyComponentInitializer.cpp -o SynergyComponentInitializer.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile SynergyComponentInitializer.cpp
    exit /b 1
)

REM Build essential Sampling components
cd ../Sampling
echo Compiling BatchCreator.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c BatchCreator.cpp -o BatchCreator.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BatchCreator.cpp
    exit /b 1
)


cd ../Analysis
echo Compiling SubjectToCSV.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c TestSubjectToCSV.cpp -o TestSubjectToCSV.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile TestSubjectToCSV.cpp
    exit /b 1
)

echo Compiling RMSE.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c RMSE.cpp -o RMSE.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile RMSE.cpp
    exit /b 1
)

cd ../AutoBalancing
echo Compiling Chromosome.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c Chromosome.cpp -o Chromosome.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile Chromosome.cpp
    exit /b 1
)

echo Compiling GeneticBalancingProcessor.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c GeneticBalancingProcessor.cpp -o GeneticBalancingProcessor.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile GeneticBalancingProcessor.cpp
    exit /b 1
)

REM Build main executable with library
cd ..
echo Linking Run.exe...

g++ %LINK_FLAGS% ^
    -I. ^
    -IElement ^
    -ISampling ^
    -ISimulator ^
    -ISimulator/yaml-cpp/include ^
    -DYAML_CPP_STATIC_DEFINE ^
    Run.cpp ^
    Log/LogSystem.o ^
    Element/BattleActionLoader.o ^
    Element/BattleActionParser.o ^
    Element/EquipmentLoader.o ^
    Element/UnitGenerator.o ^
    Element/ElementList.o ^
    Element/Stat.o ^
    Element/CounterTypeInitializer.o ^
    Element/SynergyComponentInitializer.o ^
    Sampling/BatchCreator.o ^
    Analysis/TestSubjectToCSV.o ^
    Analysis/RMSE.o ^
    AutoBalancing/Chromosome.o ^
    AutoBalancing/GeneticBalancingProcessor.o ^
    -LSimulator ^
    -lsimulator ^
    -LSimulator/yaml-cpp/mingw-build ^
    -lyaml-cpp ^
    -o Run.exe

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Run.exe
    exit /b 1
) else (
    if %DEBUG_MODE%==1 (
        echo Run.exe built successfully with DEBUG symbols!
        echo Usage: gdb ./Run.exe
    ) else (
        echo Run.exe built successfully in RELEASE mode!
    )
)