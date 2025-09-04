@echo off
echo Building Simple Direct Equipment Analysis Tool...

echo Compiling SimpleDirectAnalysis.cpp...
g++ -std=c++17 SimpleDirectAnalysis.cpp -o SimpleDirectAnalysis.exe

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Simple Direct Analysis tool built successfully!
echo Run with: .\SimpleDirectAnalysis.exe
pause
