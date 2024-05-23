# Compiler info
CXX = g++

# Flags
CXXFLAGS = -O3 -std=c++20

INCLUDES = -I ./SimulationSoftware/ \
					 -I ../Empirical/include/

all: bench ftest profile

bench:
	g++ $(CXXFLAGS) $(INCLUDES) -fopenmp -DNDEBUG -o bench ./Utility/benchmark.cpp ./SimulationSoftware/evolution.cpp

ftest:
	g++ $(CXXFLAGS) $(INCLUDES) -o ftest ./Utility/fitness_map_test.cpp ./SimulationSoftware/evolution.cpp

profile:
	g++ $(CXXFLAGS) $(INCLUDES) -pg -DNDEBUG -o profile ./Utility/profile_test.cpp ./SimulationSoftware/evolution.cpp

# Clean up the mess
clean:
	rm -f bench
	rm -f ftest
	rm -f profile
