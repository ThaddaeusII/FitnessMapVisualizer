#ifndef WEBSITE_HPP
#define WEBSITE_HPP

#include "evolution.h"
#include "population_graph.hpp"
#include "emp/web/Document.hpp"

namespace UI = emp::web;

// Class to represent the whole website
class Website
{
private:
  double csize; // Population simulation canvas size (width and height)
  Population p; // Object to simulate population

  UI::Document doc; // HTML doc for the website
  PopulationGraph pg; // Animated graph for visualizing population and fitness landscape
  UI::Table fmap; // Table for showing fitness landscape

public: 
  // Website constructor
  Website() :
    csize(500.0),
    p(10000, 0.1, 6, 5),
    doc("emp_base"),
    pg(csize, doc, p),
    fmap(1, 1, "fmap")
  {
    // Setup fitness map editor / visualizer
    //doc << fmap;
  }
};

#endif
