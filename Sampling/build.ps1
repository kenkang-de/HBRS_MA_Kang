# PowerShell build script for RunTests
param(
    [switch]$Clean,
    [switch]$Rebuild
)

$CXX = "g++"
$CXXFLAGS = "-std=c++17", "-Wall", "-O2"
$INCLUDES = @("-I.", "-I../Simulator", "-I../Analysis", "-I../Simulator/yaml-cpp/include")
$LIBS = @("-L../Simulator/yaml-cpp/build", "-lyaml-cpp")

# Source files
$SIM_SOURCES = @(
    "../Simulator/Simulator.cpp",
    "../Simulator/BattleManager.cpp", 
    "../Simulator/TurnManager.cpp",
    "../Simulator/Battlefield.cpp",
    "../Simulator/Team.cpp",
    "../Simulator/Unit.cpp",
    "../Simulator/UnitGenerator.cpp",
    "../Simulator/Stat.cpp",
    "../Simulator/EquipmentManager.cpp",
    "../Simulator/BattleAction.cpp",
    "../Simulator/BoonAction.cpp",
    "../Simulator/BattleActionLoader.cpp",
    "../Simulator/BattleActionParser.cpp",
    "../Simulator/ActionLibrary.cpp",
    "../Simulator/TargetManager.cpp"
)

$ANALYSIS_SOURCES = @(
    "../Analysis/ElementAnalysis.cpp",
    "../Analysis/TestSubject.cpp"
)

$SAMPLING_SOURCES = @("RunTests.cpp")

$ALL_SOURCES = $SIM_SOURCES + $ANALYSIS_SOURCES + $SAMPLING_SOURCES
$TARGET = "RunTests.exe"

function Get-ObjectFile($source) {
    return [System.IO.Path]::ChangeExtension($source, ".o")
}

function Test-NeedsRecompile($source, $object) {
    if (!(Test-Path $object)) { return $true }
    return (Get-Item $source).LastWriteTime -gt (Get-Item $object).LastWriteTime
}

# Clean if requested
if ($Clean -or $Rebuild) {
    Write-Host "Cleaning build artifacts..."
    foreach ($source in $ALL_SOURCES) {
        $obj = Get-ObjectFile $source
        if (Test-Path $obj) { Remove-Item $obj }
    }
    if (Test-Path $TARGET) { Remove-Item $TARGET }
}

# Compile changed files only
$compiledCount = 0
$skippedCount = 0

foreach ($source in $ALL_SOURCES) {
    $obj = Get-ObjectFile $source
    
    if (Test-NeedsRecompile $source $obj) {
        Write-Host "Compiling $source..." -ForegroundColor Green
        $args = $CXXFLAGS + $INCLUDES + @("-c", $source, "-o", $obj)
        & $CXX @args
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Compilation failed for $source"
            exit 1
        }
        $compiledCount++
    } else {
        Write-Host "Skipping $source (up to date)" -ForegroundColor Yellow
        $skippedCount++
    }
}

Write-Host "Compiled: $compiledCount files, Skipped: $skippedCount files"

# Link if target doesn't exist or any object is newer
$needsLink = !(Test-Path $TARGET)
if (!$needsLink) {
    $targetTime = (Get-Item $TARGET).LastWriteTime
    foreach ($source in $ALL_SOURCES) {
        $obj = Get-ObjectFile $source
        if ((Get-Item $obj).LastWriteTime -gt $targetTime) {
            $needsLink = $true
            break
        }
    }
}

if ($needsLink) {
    Write-Host "Linking $TARGET..." -ForegroundColor Cyan
    $objects = $ALL_SOURCES | ForEach-Object { Get-ObjectFile $_ }
    $args = $objects + @("-o", $TARGET) + $LIBS
    & $CXX @args
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Linking failed"
        exit 1
    }
    Write-Host "Build complete: $TARGET" -ForegroundColor Green
} else {
    Write-Host "$TARGET is up to date" -ForegroundColor Yellow
}
