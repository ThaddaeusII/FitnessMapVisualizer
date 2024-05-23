#include "evolution.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <string>

/*
 * Constructor for organism
 * Arguments: x gene, y gene
 * Returns: Organism
 */
Organism::Organism(int x, int y)
{
  this->x = x;
  this->y = y;
}

/*
 * Function to get the fitness for the current organism from a fitness map
 * Arguments: None
 * Returns: Nothing
 */
void Organism::getFitness(double ** fitness_map)
{
  if (fitness_map == NULL)
  {
    std::cout << "Fitness map not set!" << std::endl;
    return;
  }
  fit = fitness_map[y][x];
}

/* 
 * Function to mutate an Organism
 * Arguments: mutation direction
 * Returns: Nothing
 */
void Organism::mutate(int dir, int xlim, int ylim)
{
  switch(dir)
  {
  case 0: // increase gene x
    if (x < xlim - 1)
      x += 1;
    break;
  case 1: // decrease gene x
    if (x > 0)
      x -= 1;
    break;
  case 2: // increase gene y
    if (y < ylim - 1)
      y += 1;
    break;
  case 3: // decrease gene y
    if (y > 0)
      y -= 1;
    break;
  default:
    std::cout << "ERROR: Invalid mutation direction (" << dir << ")" << std::endl;
  }
}

/*
 * Constructs a Population of Organsims
 * Arguments: size of population, mutation rate, directory to save data in,
 *            and fitness map file to load from
 * Returns: Population
 */
Population::Population(int n, double m, std::string directory, std::string fitness)
{
  this->n = n; // Population count (maybe keep record of inital?)
  this->m = m; // Mutation chance (would be cool if it changed)
  this->directory = directory.append("gen_"); // File path to save to, gen_#, # is determined later
  gen = 0; // Generation number, starts at 0
  fitness_map = new double*[MAX_GENE_SIZE];
  for (int i = 0; i < MAX_GENE_SIZE; ++i)
    fitness_map[i] = new double[MAX_GENE_SIZE];

  // Initialize random distributions
  std::random_device rd;
  mt.seed(rd());
  ud_dir = std::uniform_int_distribution<int>(0, 3); // What direction for mutation
  ud_par = std::uniform_int_distribution<int>(0, n - 1); // Select parent to check
  ud_real = std::uniform_real_distribution<double>(0.0, 1.0); // Probability of offspring based on fitness

  // Load fitness function
  loadFitnessFunction(fitness);
  
  // Initialize population at center of current genetic map
  for (int i = 0; i < n; ++i)
  {
    Organism o(xlim / 2, ylim / 2);
    o.getFitness(fitness_map);
    pop.push_back(o);
  }
}

/*
 * Deconstructor for Population, gets rid of fitness map array
 * Arguments: None
 * Returns: Nothing
 */
Population::~Population()
{
  for (int i = 0; i < ylim; ++i)
    delete[] fitness_map[i];
  delete[] fitness_map;
}

/*
 * Function that will simulate generations of a population
 * Arguments: How many generations, tournament size to be used, and flag for saving
 * Returns: Nothing
 */
void Population::evolve(int generations, int tournament_size, bool save)
{
  // Ensure that there is a fitness function and population
  if (fitness_map == NULL || pop.size() == 0)
  {
    std::cout << "Cannot evolve with an empty population or no fitness map" << std::endl;
    return;
  }

  // Track generations
  // std::cout << "Generation " << gen << ", " << n << " organisms!" << std::endl;
  std::string file;
  if (save)
  {
    file = directory + std::to_string(gen) + ".txt";
    savePopulation(file);
  }

  for (int i = 0; i < generations; ++i)
  {
    // Next generation
    ++gen;

    // Create children based on fitness, track mutations
    selectionTournament(tournament_size);
    
    //if ((i + 1) % 100 == 0)
    //{
    //  std::cout << "Generation " << gen << " complete!" << std::endl;
    //}

    // Save current generation
    if (save)
    {
      file = directory + std::to_string(gen) + ".txt";
      savePopulation(file);
    }
  }
}

/*
 * Function that creates a new generation via tournament selection
 * Arguments: Tournament size
 * Returns: Nothing
 */ 
void Population::selectionTournament(int t)
{
  std::vector<Organism> new_pop; // Population of new generation

  for (int i = 0; i < n; ++i)
  {
    // Parent selection process, select t organisms for tournament
    int max_parent = random_par();
    for (int i = 0; i < t - 1; ++i)
    {
      int parent = random_par();
      max_parent = (pop[parent].fit > pop[max_parent].fit) ? parent : max_parent;
    }

    // Create new offspring at parent
    Organism o(pop[max_parent].x, pop[max_parent].y);

    // Check if there's a mutation
    double mutation_check = random_real();
    if (mutation_check < m)
      o.mutate(random_dir(), xlim, ylim);

    // Get organism's fitness
    o.getFitness(fitness_map);

    // Save to new population
    new_pop.push_back(o);
  }

  // Change to new population
  pop = new_pop;
}

/*
 * Function to get a random direction.
 * Arguments: None
 * Returns: Random number in {0,1,2,3}
 */ 
int Population::random_dir()
{
  return ud_dir(mt);
}

/*
 * Function to select a random parent
 * Arguments: None
 * Returns: Number in range [0, N]
 */ 
int Population::random_par()
{
  return ud_par(mt);
}

/*
 * Function to get a random chance
 * Arguments: None
 * Returns: Decimal in range [0.0, 1.0]
 */ 
double Population::random_real()
{
  return ud_real(mt);
}

/*
 * Function to save a Population to file
 * Arguments: Filepath/name to save to
 * Returns: Nothing
 */ 
void Population::savePopulation(std::string file)
{
  std::ofstream f(file);
  
  f << "N " << n << std::endl;
  f << "M " << m << std::endl;
  f << "G " << gen << std::endl;

  for (int i = 0; i < n; ++i)
    f << pop[i].x << " " << pop[i].y << " " << pop[i].fit << std::endl;

  f.close();
}

/*
 * Function to load a Population from file
 * Arguments: Filepath/name to load from
 * Returns: Nothing
 */ 
void Population::loadPopulation(std::string file)
{
  pop.clear();

  std::ifstream f(file);

  char temp;
  f >> temp >> n;
  f >> temp >> m;
  f >> temp >> gen;

  for (int i = 0; i < n; ++i)
  {
    int x = 0;
    int y = 0;
    double fit = 0;
    
    f >> x >> y >> fit;
    
    Organism o(x, y);
    o.fit = fit;
    pop.push_back(o);
  }

  f.close();
}

/*
 * Function to load a fitness function from file (2D array)
 * Arguments: Filepath/name to load from
 * Returns: Nothing
 */ 
void Population::loadFitnessFunction(std::string file)
{
  std::ifstream f(file);

  // Get fitness map size
  int maxfit;
  double fitspace;
  f >> xlim >> ylim >> maxfit >> fitspace;

  // If fitness map is larger than allowed
  if (xlim > MAX_GENE_SIZE || ylim > MAX_GENE_SIZE)
  {
    std::cout << "Fitness map exceeds maximum allowed size!" << std::endl;
    return;
  }

  // Load fitness matrix
  for (int i = 0; i < ylim; ++i)
    for (int j = 0; j < xlim; ++j)
      f >> fitness_map[i][j];

  f.close();
}

/*
 * Function to display the fitness map matrix
 * Arguments: None
 * Returns: Nothing
 */
void Population::displayFitnessFunction()
{
  for (int i = 0; i < ylim; ++i)
  {
    for (int j = 0; j < xlim; ++j)
    {
      std::cout << int(fitness_map[i][j]) << " ";
    }
       std::cout << std::endl;
  }
}
