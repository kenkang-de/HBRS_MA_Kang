@echo off
echo Building Simulator library...

cd Simulator

g++ -std=c++17 -I. -I.. -Iyaml-cpp/include -DYAML_CPP_STATIC_DEFINE -c BattleAction.cpp -o BattleAction.o
g++ -std=c++17 -I. -I.. -c Stat.cpp -o Stat.o
g++ -std=c++17 -I. -I.. -c Unit.cpp -o Unit.o
g++ -std=c++17 -I. -I.. -c Battlefield.cpp -o Battlefield.o
g++ -std=c++17 -I. -I.. -c BattleManager.cpp -o BattleManager.o
g++ -std=c++17 -I. -I.. -c TargetManager.cpp -o TargetManager.o
g++ -std=c++17 -I. -I.. -c TurnManager.cpp -o TurnManager.o
g++ -std=c++17 -I. -I.. -c EquipmentManager.cpp -o EquipmentManager.o
g++ -std=c++17 -I. -I.. -c Team.cpp -o Team.o
g++ -std=c++17 -I. -I.. -c Simulator.cpp -o Simulator.o
g++ -std=c++17 -I. -I.. -c BoonAction.cpp -o BoonAction.o
g++ -std=c++17 -I. -I.. -c TempBoonAction.cpp -o TempBoonAction.o
g++ -std=c++17 -I. -I.. -c GlobalAction.cpp -o GlobalAction.o
g++ -std=c++17 -I. -I.. -c ActionLibrary.cpp -o ActionLibrary.o

ar rcs libsimulator.a BattleAction.o Stat.o Unit.o Battlefield.o BattleManager.o TargetManager.o TurnManager.o EquipmentManager.o Team.o Simulator.o BoonAction.o TempBoonAction.o GlobalAction.o ActionLibrary.o

cd..
echo Simulator library built successfully!