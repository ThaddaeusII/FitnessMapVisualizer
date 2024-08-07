#include "evolution.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc != 8)
  {
    std::cout << "Invalid arguments!" << std::endl;
    return 1;
  }
  int population = atoi(argv[1]);
  double mutation_rate = atof(argv[2]);
  int generations = atoi(argv[3]);
  int tournament_size = atoi(argv[4]);
  std::string fitness_map(argv[5]);
  int xstart = atoi(argv[6]);
  int ystart = atoi(argv[7]);

  //Population p(population, mutation_rate, xstart, ystart);
  //p.loadFitnessFunction(fitness_map);
  //p.evolve(generations, 't', tournament_size, false, "./TestData/");
  //p.savePopulation("./fitness_test_results_tournament.txt");

  Population q(population, mutation_rate, xstart, ystart);
  q.loadFitnessFunction(fitness_map);
  q.evolve(generations, 'r', 0, false, "./TestData/");
  q.savePopulation("./fitness_test_results_roulette.txt");

  return 0;
}
