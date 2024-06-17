#ifndef WEBSITE_HPP
#define WEBSITE_HPP

#include "evolution.h"
#include "population_graph.hpp"
#include "fitness_editor.hpp"

namespace UI = emp::web;

// Class to represent the whole website
class Website
{
private:
  double csize; // Population simulation canvas size (width and height)
  Population pop; // Object to simulate population

  UI::Document doc; // HTML doc for the website
  UI::Table layout; // Table for setting layout of objects
  PopulationGraph pg; // Animated graph for visualizing population and fitness landscape
  SimulationEditor se; // Allow for easy modification of population and fitness
  UI::Table fmap; // Table for showing fitness landscape

public: 

  // Website constructor
  Website() :
    csize(500.0),
    pop(10000, 0.1, 6, 5),
    doc("emp_base"),
    layout(3, 3, "layout"),
    pg(csize, doc, pop, layout),
    se(csize, doc, pop, layout),
    fmap(1, 1, "fmap")
  {
    // Setup page layout
    doc << layout;
    layout.GetCell(1, 1) << fmap;
    layout.CellsCSS("border", "1px solid black");
    layout.CellsCSS("padding", "5px");

    // Setup fitness map editor / visualizer
    UpdateFitnessTable();
  }

  void UpdateFitnessTable()
  {
    fmap.Resize(pop.ylim, pop.xlim);
    for (int i = 0; i < pop.ylim; ++i)
    {
      for (int j = 0; j < pop.xlim; ++j)
      {
        fmap.GetCell(j, i).ClearChildren() << pop.fitness_map[j][i];
      }
    }
    fmap.CellsCSS("border", "1px solid black");
    fmap.CellsCSS("padding", "5px");
    fmap.Redraw();
  }
};

#endif
