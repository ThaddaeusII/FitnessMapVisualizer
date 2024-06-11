#include "evolution.h"
#include <iostream>
#include <fstream>
#include "emp/datastructs/IndexMap.hpp"

/*
 * Default constructor for organism
 * Arguments: None
 * Returns: Organism
 */
Organism::Organism() : x(0), y(0), fit(0.0)
{
}

/*
 * Constructor for organism if coordinates known
 * Arguments: x gene, y gene
 * Returns: Organism
 */
Organism::Organism(int x, int y) : x(x), y(y), fit(0.0)
{ 
}

/*
 * Function to get the fitness for the current organism from a fitness map
 * Arguments: None
 * Returns: Nothing
 */
void Organism::getFitness(emp::array<emp::array<double, MAX_GENE_SIZE>, MAX_GENE_SIZE> &fitness_map)
{
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
Population::Population(int n, double m, int xstart, int ystart) : n(n), m(m)
{
  if (n > MAX_POP_SIZE)
  {
    this->n = MAX_POP_SIZE;
    n = MAX_POP_SIZE;
    std::cout << "Population size exceeds limit, limiting to " << MAX_POP_SIZE << "!" << std::endl;
  }

  gen = 0; // Generation number, starts at 0
  
  // Initialize fitness map to all 0s
  for (int i = 0; i < MAX_GENE_SIZE; ++i)
    for (int j = 0; j < MAX_GENE_SIZE; ++j)
      fitness_map[i][j] = 0;


  // Adjust xstart and ystart if necessary
  if (xstart == -1)
    xstart = xlim / 2;
  if (ystart == -1)
    ystart = ylim / 2;
  
  // Initialize population at center of current genetic map
  first_pop = true;
  for (int i = 0; i < n; ++i)
  {
    pop1[i].x = xstart;
    pop1[i].y = ystart;
    pop1[i].getFitness(fitness_map);
  }

  // Initialize roulette map with size n
  roulette_map = emp::IndexMap(n);
}

/*
 * Function that will simulate generations of a population
 * Arguments: How many generations, flag for selection method, tournament size to be used, and flag for saving
 * Returns: Nothing
 */
void Population::evolve(int generations, char selection, int tournament_size, bool save_all, std::string save_dir)
{
  std::string file_start = save_dir.append("gen_"); // File path to save to, gen_#, # is determined later
  
  // Ensure that there is a population
  if (n == 0)
  {
    std::cout << "Cannot evolve with an empty population" << std::endl;
    return;
  }

  // Track generations
  // std::cout << "Generation " << gen << ", " << n << " organisms!" << std::endl;
  std::string file;
  if (save_all)
  {
    file = file_start + std::to_string(gen) + ".txt";
    savePopulation(file);
  }

  for (int i = 0; i < generations; ++i)
  {
    // Next generation
    ++gen;

    // Create children based on fitness, track mutations
    switch(selection)
    {
    case 't':
      selectionTournament(tournament_size);
      break;
    case 'r':
      selectionRoulette();
      break;
    default:
      selectionTournament(7);
    }
    
    //if ((i + 1) % 100 == 0)
    //{
    //  std::cout << "Generation " << gen << " complete!" << std::endl;
    //}

    // Save current generation
    if (save_all)
    {
      file = file_start + std::to_string(gen) + ".txt";
      savePopulation(file);
    }
  }
}

/*
 * Function to reset a population
 * Arguments: None
 * Returns: Nothing
 */
void Population::reset(int x, int y)
{
  gen = 0;
  for (int i = 0; i < n; ++i)
  {
    pop1[i].x = x;
    pop1[i].y = y;
    pop1[i].getFitness(fitness_map);
    pop2[i].x = x;
    pop2[i].y = y;
    pop2[i].getFitness(fitness_map);
  }
}

/*
 * Function that creates a new generation via tournament selection
 * Arguments: Tournament size
 * Returns: Nothing
 */ 
void Population::selectionTournament(int t)
{
  if (first_pop)
  {
    for (int i = 0; i < n; ++i)
    {
      // Parent selection process, select t organisms for tournament
      int max_parent = rng.GetInt(0, n);
      for (int i = 0; i < t - 1; ++i)
      {
        int parent = rng.GetInt(0, n);
        max_parent = (pop1[parent].fit > pop1[max_parent].fit) ? parent : max_parent;
      }

      // Create child
      pop2[i].x = pop1[max_parent].x;
      pop2[i].y = pop1[max_parent].y;

      // Check if there's a mutation
      if (rng.P(m))
        pop2[i].mutate(rng.GetInt(0, 4), xlim, ylim);
    
      // Get organism's fitness
      pop2[i].getFitness(fitness_map);
    }
  }
  else // If current population is in pop2
  {
    for (int i = 0; i < n; ++i)
    {
      // Parent selection process, select t organisms for tournament
      int max_parent = rng.GetInt(0, n);
      for (int i = 0; i < t - 1; ++i)
      {
        int parent = rng.GetInt(0, n);
        max_parent = (pop2[parent].fit > pop2[max_parent].fit) ? parent : max_parent;
      }

      // Create child
      pop1[i].x = pop2[max_parent].x;
      pop1[i].y = pop2[max_parent].y;

      // Check if there's a mutation
      if (rng.P(m))
        pop1[i].mutate(rng.GetInt(0, 4), xlim, ylim);
    
      // Get organism's (new) fitness
      pop1[i].getFitness(fitness_map);
    }
  }
  
  // Change to use other array
  first_pop = !first_pop;
}

/*
 * Function to perform roulette selection
 * Arguments: None
 * Returns: Nothing
 */
void Population::selectionRoulette()
{
  // Determine if pop1 or pop2 has current population
  if (first_pop)
  {
    // Calculate the index map (would it be faster to make a vector and adjustall or adjust n times?)
    for (int i = 0; i < n; ++i)
      roulette_map[i] = pop1[i].fit;

    // Ensure population isn't dead
    if (roulette_map.GetWeight() == 0)
    {
      std::cout << "Population is dead (pop1), can't evolve!" << std::endl;
      exit(1);
      return;
    }

    // Select parents
    for (int i = 0; i < n; ++i)
    {
      // Select random parent via roulette style
      int parent = roulette_map.Index(rng.GetDouble(0, roulette_map.GetWeight())); // Does this do what I expect?

      // Create child
      pop2[i].x = pop1[parent].x;
      pop2[i].y = pop1[parent].y;

      // Check if there's a mutation
      if (rng.P(m))
        pop2[i].mutate(rng.GetInt(0, 4), xlim, ylim);
    
      // Get organism's (new) fitness
      pop2[i].getFitness(fitness_map);
    }
  }
  else
  {
    // Calculate the index map (would it be faster to make a vector and adjustall or adjust n times?)
    for (int i = 0; i < n; ++i)
      roulette_map[i] = pop2[i].fit;

    // Ensure population isn't dead
    if (roulette_map.GetWeight() == 0)
    {
      displayFitnessFunction();
      std::cout << roulette_map[0] << std::endl;
      std::cout << "Population is dead (pop2), can't evolve!" << std::endl;
      exit(1);
      return;
    }

    // Select parents
    for (int i = 0; i < n; ++i)
    {
      // Select random parent via roulette style
      int parent = roulette_map.Index(rng.GetDouble(0, roulette_map.GetWeight())); // Does this do what I expect?

      // Create child
      pop1[i].x = pop2[parent].x;
      pop1[i].y = pop2[parent].y;

      // Check if there's a mutation
      if (rng.P(m))
        pop1[i].mutate(rng.GetInt(0, 4), xlim, ylim);
    
      // Get organism's (new) fitness
      pop1[i].getFitness(fitness_map);
    }
  }

  // Swap which array is active
  first_pop = !first_pop;
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

  if (first_pop)
    for (int i = 0; i < n; ++i)
      f << pop1[i].x << " " << pop1[i].y << " " << pop1[i].fit << std::endl;
  else
    for (int i = 0; i < n; ++i)
      f << pop2[i].x << " " << pop2[i].y << " " << pop2[i].fit << std::endl;

  f.close();
}

/*
 * Function to load a Population from file
 * Arguments: Filepath/name to load from
 * Returns: Nothing
 */ 
void Population::loadPopulation(std::string file)
{
  std::ifstream f(file);

  char temp;
  f >> temp >> n;
  f >> temp >> m;
  f >> temp >> gen;

  if (n > MAX_POP_SIZE)
  {
    n = MAX_POP_SIZE;
    std::cout << "Population size exceeds limit, limiting to " << MAX_POP_SIZE << "!" << std::endl;
  }

  first_pop = true;
  for (int i = 0; i < n; ++i)
    f >> pop1[i].x >> pop1[i].y >> pop1[i].fit;

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
  double maxfit;
  double fitspace;
  f >> xlim >> ylim >> maxfit >> fitspace;

  // If fitness map is larger than allowed
  if (xlim > MAX_GENE_SIZE || ylim > MAX_GENE_SIZE)
  {
    std::cout << "Fitness map exceeds maximum allowed size!" << std::endl;
    std::cout << xlim << " || " << ylim << " > " << MAX_GENE_SIZE << std::endl;
    std::cout << "From: " << file << std::endl;
    return;
  }

  // Load fitness matrix
  for (int i = 0; i < ylim; ++i)
    for (int j = 0; j < xlim; ++j)
      f >> fitness_map[i][j];

  f.close();

  // Update current population fitness
  if (first_pop)
    for (int i = 0; i < n; ++i)
      pop1[i].getFitness(fitness_map);
  else
    for (int i = 0; i < n; ++i)
      pop2[i].getFitness(fitness_map);
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
