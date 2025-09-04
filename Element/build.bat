@echo off
echo Building Element Library...

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

echo Element library compiled successfully!
echo Object files: EquipmentLoader.o, TeamConfiguration.o

echo Linking Element.exe...
g++ -std=c++17 -I. -I.. TestElement.cpp EquipmentLoader.o TeamConfiguration.o ../Simulator/Stat.cpp -o Element.exe
if %ERRORLEVEL% EQU 0 (
    echo Element.exe created successfully!
) else (
    echo Failed to create Element.exe
    exit /b 1
)

pause
