@echo off
echo Building Run.exe...

REM Always build/rebuild Simulator library to ensure it's up to date
echo Building Simulator library...
call Simulator\build_simulator_lib.bat
if %ERRORLEVEL% NEQ 0 (
    echo Failed to build Simulator library
    exit /b 1
)

cd Log
echo Compillig LogSystem.cpp
g++ -std=c++17 -I. -I.. -I../Simulator -c LogSystem.cpp -o LogSystem.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile LogSystem.cpp
    exit /b 1
)
cd..

REM Build Element components first
cd Element
echo Compiling BattleActionLoader.cpp...
g++ -std=c++17 -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleActionLoader.cpp -o BattleActionLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleActionLoader.cpp
    exit /b 1
)

echo Compiling ElementList.cpp...
g++ -std=c++17 -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c ElementList.cpp -o ElementList.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile ElementList.cpp
    exit /b 1
)

echo Compiling BattleActionParser.cpp...
g++ -std=c++17 -I. -I.. -I../Simulator -c BattleActionParser.cpp -o BattleActionParser.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleActionParser.cpp
    exit /b 1
)

echo Compiling EquipmentLoader.cpp...
g++ -std=c++17 -I. -I.. -I../Simulator -c EquipmentLoader.cpp -o EquipmentLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile EquipmentLoader.cpp
    exit /b 1
)

echo Compiling UnitGenerator.cpp...
g++ -std=c++17 -I. -I.. -I../Simulator -c UnitGenerator.cpp -o UnitGenerator.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile UnitGenerator.cpp
    exit /b 1
)

REM Build essential Sampling components
cd ../Sampling
echo Compiling BatchCreator.cpp...
g++ -std=c++17 -I. -I.. -c BatchCreator.cpp -o BatchCreator.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BatchCreator.cpp
    exit /b 1
)

REM Build main executable with library
cd ..
echo Linking Run.exe...

g++ -std=c++17 ^
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
    Sampling/BatchCreator.o ^
    -LSimulator ^
    -lsimulator ^
    -LSimulator/yaml-cpp/mingw-build ^
    -lyaml-cpp ^
    -o Run.exe

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Run.exe
    exit /b 1
) else (
    echo Run.exe built successfully with Simulator library!
)