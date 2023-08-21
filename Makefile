# CONSTANTS

COMPILATION_OPTS = -std=c++17 -lstdc++ -Wall -Wno-sign-compare -O3 -g
APPLICATION_NAME = app
SOURCES_DIR = src
BASE_SOURCES = $(wildcard $(SOURCES_DIR)/main/util/*.cpp $(SOURCES_DIR)/main/Database/*.cpp $(SOURCES_DIR)/main/instance/*.cpp $(SOURCES_DIR)/main/schedule/*.cpp $(SOURCES_DIR)/main/neighbourhood/*.cpp $(SOURCES_DIR)/main/*.cpp wildcard $(SOURCES_DIR)/*.cpp)
ALL_SOLVERS_SOURCES = $(wildcard $(SOURCES_DIR)/main/solver/*.cpp)
SOURCES_TEST = $(wildcard $(BASE_SOURCES) $(ALL_SOLVERS_SOURCES) $(SOURCES_DIR)/test/*.cpp $(SOURCES_DIR)/test/instance/*.cpp $(SOURCES_DIR)/test/schedule/*.cpp $(SOURCES_DIR)/test/neighbourhood/*.cpp $(SOURCES_DIR)/test/solver/*.cpp $(SOURCES_DIR)/test/util/*.cpp)
OUTPUT_DIR = ./output

all: app

app: compile generalBuild

compile: clean
	@echo "Compiling application..."
	@mkdir output	
	@mkdir output/log
	g++ $(COMPILATION_OPTS) $(BASE_SOURCES) $(ALL_SOLVERS_SOURCES) -o $(OUTPUT_DIR)/$(APPLICATION_NAME)
	@echo "# Done!"

test: compileTest generalBuild
	
compileTest: clean
	@echo
	@echo "Compiling for test..."
	@mkdir output	
	g++ $(COMPILATION_OPTS) $(SOURCES_TEST) -o $(OUTPUT_DIR)/$(APPLICATION_NAME)
	@echo "# Done!"

clean:
	@echo "Cleaning..."
	@rm -rf output
	@echo "# All clean!"

generalBuild:
	@echo
	@echo "Building application..."
	# COPYING INSTANCES TO OUTPUT DIRECTORY
	@cp -r ./instances/. output
	@echo "Process completed!"
