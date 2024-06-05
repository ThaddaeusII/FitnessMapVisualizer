#ifndef WEBSITE_H
#define WEBSITE_H

#include "evolution.h"
#include "emp/web/web.hpp"
#include "emp/web/Animate.hpp"

namespace UI = emp::web;

// Class to represent the whole website
class Website
{
private:
  double csize; // Population simulation canvas size (width and height)

  UI::Document doc; // HTML doc for the website
  UI::Canvas fscape; // Fitness landscape, has population 
  UI::Table fmap; // Table for showing fitness landscape
  UI::Animate fanim; // Animation for fitness landscape

  emp::Random rng; // Random number generator
  Population p; // Object to simulate population

  void FitnessAnim(double time); // Function that animates the fitness landscape
  void DrawFitnessMap(); // Function to draw the fitness landscape grid
  void DrawPopulation(); // Function to draw the current population

public:
  Website(); // Constructor
};


#endif
