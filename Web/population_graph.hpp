#ifndef POPULATION_GRAPH_HPP
#define POPULATION_GRAPH_HPP

#include "emp/web/TextArea.hpp"
#include "evolution.h"
#include "emp/web/Document.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/web/Animate.hpp"

namespace UI = emp::web;

class PopulationGraph : public UI::Animate
{
private:
  double csize; // Size of canvas
  double time_since_last_evolve; // Time since last evolution, used for 1 evolve per second mode
  bool fast; // Run animation at fullspeed if true, else 1 fps

  UI::Document &doc; // Reference to website document
  UI::Canvas fscape; // Canvas to draw on
  UI::Table localLayout; // Track local elements layout
  UI::Button startBut; // Button for start/pausing evolution
  UI::Button resetBut; // Button for resetting population
  UI::Button speedBut; // Button for changing display speed
  UI::TextArea popTA; // Textbox for updating population size
  UI::TextArea mutTA; // Textbox for updating mutation rate
  UI::Text fpsText; // Text to display FPS
  UI::Text genText; // Text to display current generation
  
  Population &pop; // Reference to website population
  
  emp::Random rng; // Random generator for jitter

public:
  PopulationGraph(double size, UI::Document &d, Population &p, UI::Table &layout) :
    csize(size),
    time_since_last_evolve(0.0),
    fast(true),
    doc(d),
    fscape(csize, csize, "fscape"),
    localLayout(4, 1, "pop_graph_layout"),
    pop(p)
  {
    // Setup population simulator
    pop.loadFitnessFunction("./FitnessMaps/10x10_big_vs_small_unequal_peaks.map");
    DrawFitnessMap();
    DrawPopulation();
  
    // Button for playing / stopping animation for population simulator
    startBut = UI::Button(
      [this]()
      {
        ToggleActive();
        if (GetActive()) startBut.SetLabel("Pause");
        else startBut.SetLabel("Start");
      },
      "Start",
      "start"
      );

    // Button for restarting
    resetBut = UI::Button(
      [this]()
      {
        pop.reset(6, 5); // Change this so starting pos can be changed
        Redraw();
      },
      "Reset",
      "reset"
      );

    // Button for determining animation mode
    speedBut = UI::Button(
      [this]()
      {
        if (fast) speedBut.SetLabel("Fast");
        else speedBut.SetLabel("Slow");
        fast = !fast;
      },
      "Slow",
      "speed"
      );

    // Text box for setting population
    popTA = UI::TextArea([this](const std::string &s){ return GetPop(s); }, "pop_size");
    popTA.SetSize(80, 20);
    popTA.SetText(std::to_string(pop.n));

    // Text box for setting mutation rate
    mutTA = UI::TextArea([this](const std::string &s){ return GetMut(s); }, "mut_rate");
    mutTA.SetSize(80, 20);
    mutTA.SetText(std::to_string(pop.m));

    // Display FPS and current generation of population simulator
    fpsText = UI::Text("fps");
    fpsText << " FPS = " << UI::Live( [this](){return 1000.0 / GetStepTime();} );
    genText = UI::Text("gen");
    genText << " Gen = " << UI::Live( [this](){return pop.gen;} );

    // Set layout
    layout.GetCell(0, 0) << localLayout;
    localLayout.CellsCSS("border", "1px solid black");
    localLayout.CellsCSS("padding", "5px");
    localLayout.GetCell(0, 0) << fscape;
    localLayout.GetCell(1, 0) << startBut << resetBut << speedBut;
    localLayout.GetCell(2, 0) << popTA << mutTA;
    localLayout.GetCell(3, 0) << fpsText << genText;
  }

  // Function to process user input for population, should set pop.n if valid
  void GetPop(const std::string &s)
  {
    // Get integer for n and set
    int n = stoi(s);
    pop.n = n;
    Redraw();
  }

  // Function to process user input for mutation rate, should set pop.m if valid
  void GetMut(const std::string &s)
  {
    // Get double for m and set
    double m = stod(s);
    pop.m = m;
  }

  void Redraw()
  {
    // Clear the canvas
    fscape.Clear();

    // Draw background (fitness landscape)
    DrawFitnessMap();

    // Draw updated population
    DrawPopulation();

    // Redraw FPS and Generation
    doc.Text("fps").Redraw();
    doc.Text("gen").Redraw();
  }
  
  void DoFrame()
  {
    // Update the population
    time_since_last_evolve += GetStepTime();
    if (fast)
      pop.evolve(1, 'r');
    else if (time_since_last_evolve >= 1000)
    {
      pop.evolve(1);
      time_since_last_evolve = 0.0;
    }

    Redraw();
  }
  
  // Function that draws the fitness landscape as a background
  void DrawFitnessMap()
  {
    double unit = csize / pop.xlim;
    std::string color;

    for (int i = 0; i < pop.xlim; ++i)
    {
      for (int j = 0; j < pop.ylim; ++j)
      {
        // Choose tile color based on fitness
        switch(int(pop.fitness_map[j][i]))
        {
          case 85:
            color = "purple";
            break;
          case 80:
            color = "blue";
            break;
          case 1:
            color = "grey";
            break;
          default:
            color = "black";
            break;
        }

        // Draw grid tile using assigned color
        fscape.Rect(unit * i, unit * j, unit, unit, color, "black");
      }
    }
  }

  // Function to draw the population on the fitness landscape
  void DrawPopulation()
  {
    double unit = csize / pop.xlim;

    for (int i = 0; i < pop.n; ++i)
    {
      // Add jitter
      double xjitter = rng.GetDouble(-0.4, 0.4);
      double yjitter = rng.GetDouble(-0.4, 0.4);
    
      // Choose x and y from current population, +0.5 to center on squares
      double x = ((pop.first_pop) ? pop.pop1[i].x : pop.pop2[i].x) + 0.5;
      double y = ((pop.first_pop) ? pop.pop1[i].y : pop.pop2[i].y) + 0.5;

      // Draw circles scaled to unit size for each organism
      fscape.Circle((x + xjitter) * unit, (y + yjitter) * unit, unit / 10.0, "red", "black");
    }
  }
};

#endif
