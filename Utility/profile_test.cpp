#include "evolution.h"

int main()
{
  Population p(100000, 0.01, "./", "./FitnessMaps/100x100_ring.map");
  p.evolve(1000000, 's', 7);
  return 0;
}
