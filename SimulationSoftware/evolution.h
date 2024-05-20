#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "emp/base/vector.hpp"
#include "emp/base/array.hpp"
#include "emp/math/Random.hpp"
#include <string>

const int MAX_GENE_SIZE = 100;

struct Organism
{
  int x;
  int y;
  double fit;

  Organism(int x, int y);
  void getFitness(emp::array<emp::array<double, MAX_GENE_SIZE>, MAX_GENE_SIZE> &fitness_map);
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
  emp::vector<Organism> pop;
  emp::array<emp::array<double, MAX_GENE_SIZE>, MAX_GENE_SIZE> fitness_map;

  // Population construction and destruction
  Population(int n = 10000,
             double m = 0.01,
             std::string directory = "./",
             std::string fitness = "./fitness_test.map");
  ~Population();

  // Main simulation
  void evolve(int generations = 100,
              int tournament_size = 7,
              bool save = false);
  
  // Parent selection methods
  void selectionTournament(int t);

  // File IO
  void savePopulation(std::string file);
  void loadPopulation(std::string file);
  void loadFitnessFunction(std::string file);
  
  // Display functions
  void displayFitnessFunction();
};

#endif
