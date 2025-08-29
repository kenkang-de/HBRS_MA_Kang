# PowerShell build script for Simulator
Write-Host "Building Simulator..." -ForegroundColor Green

$sources = @(
    "Simulator.cpp", "Stat.cpp", "Unit.cpp", "Team.cpp", "UnitGenerator.cpp",
    "EquipmentManager.cpp", "TurnManager.cpp", "BattleManager.cpp", 
    "BattleAction.cpp", "BattleActionLoader.cpp", "BattleActionParser.cpp",
    "TargetManager.cpp", "ActionLibrary.cpp", "Battlefield.cpp", 
    "BoonAction.cpp", "TempBoonAction.cpp"
)

$includes = "-I yaml-cpp/include"
$libs = "-L yaml-cpp/mingw-build -lyaml-cpp"
$flags = "-g -std=c++17 -DYAML_CPP_STATIC_DEFINE"

$command = "g++ $flags $($sources -join ' ') $includes $libs -o Simulator.exe"

Write-Host "Executing: $command" -ForegroundColor Yellow

try {
    Invoke-Expression $command
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build successful! Simulator.exe created." -ForegroundColor Green
    } else {
        Write-Host "Build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "Error during build: $_" -ForegroundColor Red
    exit 1
}

# Optional: Build other tools
if ($args -contains "all") {
    Write-Host "Building additional tools..." -ForegroundColor Green
    
    # Build test runner
    g++ -g RunTests.cpp -o RunTests.exe
    if ($LASTEXITCODE -eq 0) { Write-Host "RunTests.exe built successfully" -ForegroundColor Green }
    
    # Build advanced test runner
    g++ -g RunTestsAdvanced.cpp -o RunTestsAdvanced.exe
    if ($LASTEXITCODE -eq 0) { Write-Host "RunTestsAdvanced.exe built successfully" -ForegroundColor Green }
    
    # Build analyzer
    g++ -g ResultAnalyzer.cpp AnalyzeResults.cpp -o AnalyzeResults.exe
    if ($LASTEXITCODE -eq 0) { Write-Host "AnalyzeResults.exe built successfully" -ForegroundColor Green }
}
