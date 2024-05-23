#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "emp/base/array.hpp"
#include "emp/math/Random.hpp"
#include "emp/datastructs/IndexMap.hpp"
#include <string>

constexpr int MAX_GENE_SIZE = 100;
constexpr int MAX_POP_SIZE = 100000;

struct Organism
{
  int x; // X gene value
  int y; // Y gene value
  double fit; // Fitness value

  // Constructors
  Organism();
  Organism(int x, int y);

  // Function to set fitness from fitness function 
  void getFitness(emp::array<emp::array<double, MAX_GENE_SIZE>, MAX_GENE_SIZE> &fitness_map);

  // Function to mutate position in a given direction
  void mutate(int dir, int xlim, int ylim);
};

struct Population
{
  int n; // Number of organisms in population
  double m; // Mutation rate
  int gen; // Current generation number
  int xlim; // Max X gene value
  int ylim; // Max Y gene value
  std::string directory; // What directory to save files to
  
  // Random number generator
  emp::Random rng;
  
  // Organism and fitness value storage
  bool first_pop; // Using pop1 if true, else pop2 is current
  emp::array<Organism, MAX_POP_SIZE> pop1;
  emp::array<Organism, MAX_POP_SIZE> pop2;
  emp::array<emp::array<double, MAX_GENE_SIZE>, MAX_GENE_SIZE> fitness_map;
  emp::IndexMap roulette_map;

  // Population constructor
  Population(int n = 10000,
             double m = 0.01,
             std::string directory = "./",
             std::string fitness = "./FitnessMaps/fitness_test.map",
             int xstart = -1,
             int ystart = -1);

  // Main simulation
  void evolve(int generations = 100,
              char selection = 't',
              int tournament_size = 7,
              bool save = false);
  
  // Parent selection methods
  void selectionTournament(int t);
  void selectionRoulette();

  // File IO
  void savePopulation(std::string file);
  void loadPopulation(std::string file);
  void loadFitnessFunction(std::string file);
  
  // Display functions
  void displayFitnessFunction();
};

#endif
