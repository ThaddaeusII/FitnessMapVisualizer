# Compiler info
CXX = g++

# Flags
CXXFLAGS = -O3 -std=c++20

INCLUDES = -I ./SimulationSoftware/ \
					 -I ../Empirical/include/

all: bench ftest profile web

bench:
	g++ $(CXXFLAGS) $(INCLUDES) -fopenmp -DNDEBUG -o bench ./Utility/benchmark.cpp ./SimulationSoftware/evolution.cpp

ftest:
	g++ $(CXXFLAGS) $(INCLUDES) -o ftest ./Utility/fitness_map_test.cpp ./SimulationSoftware/evolution.cpp

profile:
	g++ $(CXXFLAGS) $(INCLUDES) -pg -DNDEBUG -o profile ./Utility/profile_test.cpp ./SimulationSoftware/evolution.cpp

web:
	em++ -std=c++20 -Os \
		-DNDEBUG \
		-I ./SimulationSoftware/ \
		-I ../Empirical/include/ \
		--js-library ../Empirical/include/emp/web/library_emp.js \
		-s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'allocate']" \
		-s TOTAL_MEMORY=32mb \
		-s TOTAL_STACK=64kb \
		-s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" \
		-s NO_EXIT_RUNTIME=1 \
		-o ./Web/website.js \
		./Web/main.cpp ./SimulationSoftware/evolution.cpp \
		--preload-file ./FitnessMaps/10x10_big_vs_small_unequal_peaks.map \
		--preload-file ./FitnessMaps/100x100_big_vs_small_unequal_peaks.map \
		--preload-file ./FitnessMaps/100x100_comb.map \
		--preload-file ./FitnessMaps/10x10_ring.map

# Clean up the mess
clean:
	rm -f bench
	rm -f ftest
	rm -f profile
