# Compiler info
CXX = g++

# Flags
CXXFLAGS = -O3 -std=c++20

INCLUDES = -I ./SimulationSoftware/

all: bench ftest

bench:
	g++ $(CXXFLAGS) $(INCLUDES) -fopenmp -DNDEBUG -o bench ./Utility/benchmark.cpp ./SimulationSoftware/evolution.cpp

ftest:
	g++ $(CXXFLAGS) $(INCLUDES) -o ftest ./Utility/fitness_map_test.cpp ./SimulationSoftware/evolution.cpp

# Clean up the mess
clean:
	rm -f bench
	rm -f ftest
