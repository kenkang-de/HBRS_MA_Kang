@echo off

g++ -std=c++17 -I. -I.. -c EquipmentLoader.cpp -o EquipmentLoader.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile EquipmentLoader.cpp
    pause
    exit /b 1
)

g++ -std=c++17 -I. -I.. -c TeamConfiguration.cpp -o TeamConfiguration.o
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile TeamConfiguration.cpp
    pause
    exit /b 1
)

g++ -std=c++17 -I. -I.. TestElement.cpp EquipmentLoader.o TeamConfiguration.o ../Simulator/Stat.cpp -o Element.exe
if %ERRORLEVEL% NEQ 0 (
    echo Failed to create Element.exe
    exit /b 1
)
