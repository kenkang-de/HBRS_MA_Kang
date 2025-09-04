@echo off
setlocal enabledelayedexpansion

set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -O2
set INCLUDES=-I. -I../Simulator -I../Analysis -I../Element -I../Simulator/yaml-cpp/include
set LIBS=-L../Simulator/yaml-cpp/build -lyaml-cpp
set TARGET=SamplingMain.exe

echo === Building Sampling Controller ===

REM Check if we need to rebuild
set NEED_REBUILD=0
if not exist %TARGET% set NEED_REBUILD=1

REM Quick check - if any .cpp is newer than exe, rebuild
if %NEED_REBUILD%==0 (
    for %%f in (SamplingMain.cpp SamplingController.cpp ../Element/*.cpp ../Analysis/*.cpp) do (
        if "%%f" NEQ "" (
            for /f %%i in ('forfiles /m %TARGET% /c "cmd /c echo @fdate @ftime" 2^>nul') do set TARGET_TIME=%%i
            for /f %%j in ('forfiles /m "%%f" /c "cmd /c echo @fdate @ftime" 2^>nul') do (
                if "%%j" GTR "!TARGET_TIME!" set NEED_REBUILD=1
            )
        )
    )
)

if %NEED_REBUILD%==1 (
    echo Building %TARGET%...
    %CXX% %CXXFLAGS% %INCLUDES% -o %TARGET% ^
        SamplingMain.cpp ^
        SamplingController.cpp ^
        ../Element/EquipmentLoader.cpp ^
        ../Element/TeamConfiguration.cpp ^
        ../Analysis/ElementAnalysis.cpp ^
        ../Simulator/Stat.cpp ^
        %LIBS%
    
    if %ERRORLEVEL%==0 (
        echo Build complete!
        echo.
        echo You can now run: %TARGET%
    ) else (
        echo Build failed!
        exit /b 1
    )
) else (
    echo %TARGET% is up to date, skipping build.
)
