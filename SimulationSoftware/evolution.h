#ifndef EVOLUTION_H
#define EVOLUTION_H

#include <string>
#include <vector>
#include <random>

const int MAX_GENE_SIZE = 100;

struct Organism
{
  int x;
  int y;
  double fit;

  Organism(int x, int y);
  void getFitness(double ** fitness_map);
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
  
  // Random number stuff
  std::mt19937 mt;
  std::uniform_int_distribution<int> ud_dir;
  std::uniform_int_distribution<int> ud_par;
  std::uniform_real_distribution<double> ud_real;
  
  // Organism and fitness value storage
  std::vector<Organism> pop;
  double ** fitness_map = NULL;

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

  // Random number generation
  int random_dir();
  int random_par();
  double random_real();
  
  // File IO
  void savePopulation(std::string file);
  void loadPopulation(std::string file);
  void loadFitnessFunction(std::string file);
  
  // Display functions
  void displayFitnessFunction();
};

#endif
