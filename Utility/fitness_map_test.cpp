#include "./SimulationSoftware/evolution.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc != 6)
  {
    std::cout << "Invalid arguments!" << std::endl;
    return 1;
  }
  int population = atoi(argv[1]);
  double mutation_rate = atof(argv[2]);
  int generations = atoi(argv[3]);
  int tournament_size = atoi(argv[4]);
  std::string fitness_map(argv[5]);

  Population p(population, mutation_rate, "./TestData/", fitness_map);
  p.evolve(generations, tournament_size, true);

  return 0;
}
