#include "evolution.h"

int main()
{
  Population p(10000, 0.2);
  p.loadFitnessFunction("./FitnessMaps/100x100_raised_ring.map");
  p.evolve(10000, 's', 7);
  return 0;
}
