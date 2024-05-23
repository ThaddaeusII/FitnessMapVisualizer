#include "evolution.h"
#include <chrono>
#include <iostream>
#include <numeric>
#include <ostream>
#include <vector>
#include <string>
#include <omp.h>
#include <fstream>

// Default parameters
const int TESTS = 100;
const int DEFAULT_POPULATION_SIZE = 10000;
const double DEFAULT_MUTATION_RATE = 0.01;
const int DEFAULT_GENERATIONS = 1000;
const int DEFAULT_TOURNAMENT_SIZE = 7;
const std::string DEFAULT_FITNESS_MAP = "./FitnessMaps/fitness_test.map";

template <typename T>
void SaveResults(std::vector<T> * vec, std::vector<double> * times, std::string filename)
{
  std::ofstream f(filename);
  for (int i = 0; i < vec->size(); ++i)
  {
    f << vec->at(i) << " " << times->at(i) << std::endl;
  }
  f.close();
}

void PrintProgressBar(int pos, int size, int bar_size = 100)
{
  int progress = (pos * bar_size) / size;
  
  std::cout << "[";
  for (int i = 0; i < bar_size; ++i) {
    if (i < progress) std::cout << "#";
    else std::cout << " ";
  }
  std::cout << "] " << progress + 1 << " %\r";
  std::cout.flush();
}

void TestPopulations(std::vector<int> * p, std::vector<double> * t, char selection)
{
  t->clear();
  t->resize(p->size());

  std::vector<double> iteration_times(TESTS);

  for (int i = 0; i < p->size(); ++i)
  {
    #pragma omp parallel
    {
      #pragma omp for
      for (int j = 0; j < TESTS; ++j)
      {
        auto start = std::chrono::high_resolution_clock::now();
        Population pop(p->at(i), DEFAULT_MUTATION_RATE);
        pop.loadFitnessFunction(DEFAULT_FITNESS_MAP);
        pop.evolve(DEFAULT_GENERATIONS, selection, DEFAULT_TOURNAMENT_SIZE);
  
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        iteration_times[j] = (duration.count() / 1000000000.0);
      }
    }
    t->at(i) = (std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0) / TESTS);

    std::cout << "Population " << p->at(i) << ": ";
    PrintProgressBar(i, p->size());
  }
  std::cout << std::endl;
}

void TestGenerations(std::vector<int> * g, std::vector<double> * t, char selection)
{
  t->clear();
  t->resize(g->size());

  std::vector<double> iteration_times(TESTS);

  for (int i = 0; i < g->size(); ++i)
  {
    #pragma omp parallel
    {
      #pragma omp for
      for (int j = 0; j < TESTS; ++j)
      {
        auto start = std::chrono::high_resolution_clock::now();
        Population pop(DEFAULT_POPULATION_SIZE, DEFAULT_MUTATION_RATE);
        pop.loadFitnessFunction(DEFAULT_FITNESS_MAP);
        pop.evolve(g->at(i), selection, DEFAULT_TOURNAMENT_SIZE);
  
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        iteration_times[j] = (duration.count() / 1000000000.0);
      }
    }
    t->at(i) = (std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0) / TESTS);

    std::cout << "Generations " << g->at(i) << ": ";
    PrintProgressBar(i, g->size());
  }
  std::cout << std::endl;
}

void TestTournamentSizes(std::vector<int> * s, std::vector<double> * t, char selection)
{
  t->clear();
  t->resize(s->size());

  std::vector<double> iteration_times(TESTS);

  for (int i = 0; i < s->size(); ++i)
  {
    #pragma omp parallel
    {
      #pragma omp for
      for (int j = 0; j < TESTS; ++j)
      {
        auto start = std::chrono::high_resolution_clock::now();
        Population pop(DEFAULT_POPULATION_SIZE, DEFAULT_MUTATION_RATE);
        pop.loadFitnessFunction(DEFAULT_FITNESS_MAP);
        pop.evolve(DEFAULT_GENERATIONS, selection, s->at(i));
  
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        iteration_times[j] = (duration.count() / 1000000000.0);
      }
    }
    t->at(i) = (std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0) / TESTS);

    std::cout << "Tournament size " << s->at(i) << ": ";
    PrintProgressBar(i, s->size());
  }
  std::cout << std::endl;
}

void TestMutationRates(std::vector<double> * m, std::vector<double> * t, char selection)
{
  t->clear();
  t->resize(m->size());

  std::vector<double> iteration_times(TESTS);

  for (int i = 0; i < m->size(); ++i)
  {
    #pragma omp parallel
    {
      #pragma omp for
      for (int j = 0; j < TESTS; ++j)
      {
        auto start = std::chrono::high_resolution_clock::now();
        Population pop(DEFAULT_POPULATION_SIZE, m->at(i));
        pop.loadFitnessFunction(DEFAULT_FITNESS_MAP);
        pop.evolve(DEFAULT_GENERATIONS, selection, DEFAULT_TOURNAMENT_SIZE);
  
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        iteration_times[j] = (duration.count() / 1000000000.0);
      }
    }
    t->at(i) = (std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0) / TESTS);

    std::cout << "Mutations " << m->at(i) << ": ";
    PrintProgressBar(i, m->size());
  }
  std::cout << std::endl;
}

void TestFitnessMapSizes(std::vector<int> * f, std::vector<double> * t, char selection)
{
  t->clear();
  t->resize(f->size());

  std::vector<double> iteration_times(TESTS);

  for (int i = 0; i < f->size(); ++i)
  {
    std::string s = "./FitnessMaps/TestMaps/test_" + std::to_string(f->at(i)) + "x" + std::to_string(f->at(i)) + ".map";
    #pragma omp parallel
    {
      #pragma omp for
      for (int j = 0; j < TESTS; ++j)
      {
        auto start = std::chrono::high_resolution_clock::now();
        Population pop(DEFAULT_POPULATION_SIZE, DEFAULT_MUTATION_RATE);
        pop.loadFitnessFunction(DEFAULT_FITNESS_MAP);
        pop.evolve(DEFAULT_GENERATIONS, selection, DEFAULT_TOURNAMENT_SIZE);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        iteration_times[j] = (duration.count() / 1000000000.0);
      }
    }
    t->at(i) = (std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0) / TESTS);

    std::cout << "Fitness map size " << (i + 1) << ": ";
    PrintProgressBar(i, f->size());
  }
  std::cout << std::endl;
}

int main()
{
  // Times for each test
  std::vector<double> times;

  // Population sizes to test
  std::vector<int> pop_sizes;
  for (int i = 0; i < 100; ++i)
  {
    pop_sizes.push_back((i+1) * 100);
  }

  // Run tournament selection tests
  TestPopulations(&pop_sizes, &times, 't');
  SaveResults(&pop_sizes, &times, "./BenchmarkData/population_results_tournament.txt");

  // Run roulette selection tests
  TestPopulations(&pop_sizes, &times, 'r');
  SaveResults(&pop_sizes, &times, "./BenchmarkData/population_results_roulette.txt");

  // Generation sizes to test
  std::vector<int> generations;
  for (int i = 0; i < 100; ++i)
  {
    generations.push_back((i + 1) * 10);
  }

  // Run tournament selection tests
  TestGenerations(&generations, &times, 't');
  SaveResults(&generations, &times, "./BenchmarkData/generation_results_tournament.txt");

  // Run roulette selection tests
  TestGenerations(&generations, &times, 'r');
  SaveResults(&generations, &times, "./BenchmarkData/generation_results_roulette.txt");
  
  // Tournament sizes to test
  std::vector<int> tournament_sizes;
  for (int i = 0; i < 100; ++i)
  {
    tournament_sizes.push_back((i + 1));
  }

  // Run tournament selection tests
  TestTournamentSizes(&tournament_sizes, &times, 't');
  SaveResults(&tournament_sizes, &times, "./BenchmarkData/tournament_results_tournament.txt");

  // Mutation rates to test
  std::vector<double> mutation_rates;
  for (double i = 0.1; i < 1.0; i += 0.1)
  {
    mutation_rates.push_back(i);
  }

  // Run tournament selection tests
  TestMutationRates(&mutation_rates, &times, 't');
  SaveResults(&mutation_rates, &times, "./BenchmarkData/mutation_results_tournament.txt");

  // Run roulette selection tests
  TestMutationRates(&mutation_rates, &times, 'r');
  SaveResults(&mutation_rates, &times, "./BenchmarkData/mutation_results_roulette.txt");

  // Fitness map sizes to test
  std::vector<int> fitness_map_sizes;
  for (int i = 0; i < 100; ++i)
  {
    fitness_map_sizes.push_back(i);
  }

  // Run tournament selection tests
  TestFitnessMapSizes(&fitness_map_sizes, &times, 't');
  SaveResults(&fitness_map_sizes, &times, "./BenchmarkData/fitness_map_results_tournament.txt");

  // Run roulette selection tests
  TestFitnessMapSizes(&fitness_map_sizes, &times, 'r');
  SaveResults(&fitness_map_sizes, &times, "./BenchmarkData/fitness_map_results_roulette.txt");

  return 0;
}
