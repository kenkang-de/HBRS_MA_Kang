# Makefile for RunTests
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
INCLUDES = -I. -I../Simulator -I../Analysis -I../Simulator/yaml-cpp/include
LIBS = -L../Simulator/yaml-cpp/build -lyaml-cpp

# Source directories
SIM_DIR = ../Simulator
ANALYSIS_DIR = ../Analysis

# Object files
SIM_OBJS = $(SIM_DIR)/Simulator.o $(SIM_DIR)/BattleManager.o $(SIM_DIR)/TurnManager.o \
           $(SIM_DIR)/Battlefield.o $(SIM_DIR)/Team.o $(SIM_DIR)/Unit.o \
           $(SIM_DIR)/UnitGenerator.o $(SIM_DIR)/Stat.o $(SIM_DIR)/EquipmentManager.o \
           $(SIM_DIR)/BattleAction.o $(SIM_DIR)/BoonAction.o \
           $(SIM_DIR)/BattleActionLoader.o $(SIM_DIR)/BattleActionParser.o \
           $(SIM_DIR)/ActionLibrary.o $(SIM_DIR)/TargetManager.o

ANALYSIS_OBJS = $(ANALYSIS_DIR)/ElementAnalysis.o $(ANALYSIS_DIR)/TestSubject.o

SAMPLING_OBJS = RunTests.o

ALL_OBJS = $(SIM_OBJS) $(ANALYSIS_OBJS) $(SAMPLING_OBJS)

# Target executable
TARGET = RunTests.exe

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(ALL_OBJS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(ALL_OBJS) -o $@ $(LIBS)

# Compile individual object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(ALL_OBJS) $(TARGET)

# Force rebuild
rebuild: clean all

# Quick build (only changed files)
quick: $(TARGET)

.PHONY: all clean rebuild quick
