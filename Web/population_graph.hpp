#ifndef POPULATION_GRAPH_HPP
#define POPULATION_GRAPH_HPP

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
  
  Population &pop; // Reference to website population
  
  emp::Random rng; // Random generator for jitter

public:
  PopulationGraph(double size, UI::Document &d, Population &p) :
    csize(size),
    time_since_last_evolve(0.0),
    fast(true),
    doc(d),
    fscape(doc.AddCanvas(csize, csize, "fscape")),
    pop(p)
  {
    // Setup population simulator
    pop.loadFitnessFunction("./FitnessMaps/10x10_big_vs_small_unequal_peaks.map");
    DrawFitnessMap();
    DrawPopulation();
    doc << "<br>";
  
    // Button for playing / stopping animation for population simulator
    doc.AddButton(
      [this]()
      {
        ToggleActive();
        auto but = doc.Button("start");
        if (GetActive()) but.SetLabel("Pause");
        else but.SetLabel("Start");
      },
      "Start",
      "start"
      );

    // Button for restarting
    doc.AddButton(
      [this]()
      {
        auto but = doc.Button("reset");
        pop.reset(6, 5); // Change this so starting pos can be changed
        Redraw();
      },
      "Reset",
      "reset"
      );

    // Button for determining animation mode
    doc.AddButton(
      [this]()
      {
        auto but = doc.Button("speed");
        if (fast) but.SetLabel("Fast");
        else but.SetLabel("Slow");
        fast = !fast;
      },
      "Slow",
      "speed"
      );

    // Display FPS and current generation of population simulator
    doc << "<br>";
    doc << UI::Text("fps") << " FPS = " << UI::Live( [this](){return 1000.0 / GetStepTime();} );
    doc << UI::Text("gen") << " Gen = " << UI::Live( [this](){return pop.gen;} );
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
      pop.evolve(1);
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
