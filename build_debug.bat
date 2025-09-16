@echo off
echo Building Run.exe with debug symbols...

REM Set debug flags
set COMPILE_FLAGS=-std=c++17 -g -O0
set LINK_FLAGS=-std=c++17 -g -O0

@echo off
echo Building Run.exe with debug symbols...

REM Always build/rebuild Simulator library to ensure it's up to date
echo Building Simulator library...
call Simulator\build_simulator_lib.bat
if %ERRORLEVEL% NEQ 0 (
    echo Failed to build Simulator library
    exit /b 1
)

REM Add -g for debug symbols and -O0 to disable optimizations
REM Build Element components first
cd Element
echo Compiling BattleActionLoader.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I../Simulator -I../Simulator/yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleActionLoader.cpp -o BattleActionLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleActionLoader.cpp
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

REM Build essential Sampling components
cd ../Sampling
echo Compiling BatchCreator.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c BatchCreator.cpp -o BatchCreator.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BatchCreator.cpp
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
    Element/BattleActionLoader.o ^
    Element/BattleActionParser.o ^
    Element/EquipmentLoader.o ^
    Element/UnitGenerator.o ^
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
    echo Run.exe built successfully with debug symbols!
)