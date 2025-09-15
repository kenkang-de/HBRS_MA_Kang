@echo off
echo Building Run.exe with debug symbols...

REM Set debug flags
set COMPILE_FLAGS=-std=c++17 -g -O0
set LINK_FLAGS=-std=c++17 -g -O0

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

REM Build essential Simulator components
cd ../Simulator
echo Compiling Stat.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c Stat.cpp -o Stat.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile Stat.cpp
    exit /b 1
)

echo Compiling GlobalAction.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c GlobalAction.cpp -o GlobalAction.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile GlobalAction.cpp
    exit /b 1
)

echo Compiling Unit.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c Unit.cpp -o Unit.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile Unit.cpp
    exit /b 1
)

echo Compiling BoonAction.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c BoonAction.cpp -o BoonAction.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BoonAction.cpp
    exit /b 1
)

echo Compiling TempBoonAction.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c TempBoonAction.cpp -o TempBoonAction.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile TempBoonAction.cpp
    exit /b 1
)

echo Compiling ActionLibrary.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -c ActionLibrary.cpp -o ActionLibrary.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile ActionLibrary.cpp
    exit /b 1
)

echo Compiling BattleAction.cpp...
g++ %COMPILE_FLAGS% -I. -I.. -I yaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleAction.cpp -o BattleAction.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile BattleAction.cpp
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

REM Build main executable with ALL required object files
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
    Simulator/GlobalAction.o ^
    Simulator/ActionLibrary.o ^
    Simulator/BattleAction.o ^
    Simulator/BoonAction.o ^
    Simulator/TempBoonAction.o ^
    Simulator/Stat.o ^
    Simulator/Unit.o ^
    -LSimulator/yaml-cpp/mingw-build ^
    -lyaml-cpp ^
    -o Run.exe

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Run.exe
    exit /b 1
) else (
    echo Run.exe built successfully with debug symbols!
)