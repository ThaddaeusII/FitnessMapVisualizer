#ifndef WEBSITE_HPP
#define WEBSITE_HPP

#include "evolution.h"
#include "emp/web/Document.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/web/Animate.hpp"

namespace UI = emp::web;

class PopulationGraph : public UI::Animate
{
private:
  double csize; // Size of canvas

  UI::Document &doc; // Reference to website document
  UI::Canvas fscape; // Canvas to draw on
  
  Population &pop; // Reference to website population
  
  emp::Random rng; // Random generator for jitter

public:
  PopulationGraph(double size, UI::Document &d, Population &p) :
    csize(size),
    doc(d),
    fscape(doc.AddCanvas(csize, csize, "fscape")),
    pop(p)
  {
    // Setup population simulator
    pop.loadFitnessFunction("./FitnessMaps/10x10_big_vs_small_unequal_peaks.map");
    DrawFitnessMap();
    DrawPopulation();
  
    // Button for playing / stopping animation for population simulator
    doc << "<br>";
    doc.AddButton(
      [this]()
      {
        ToggleActive();
        auto but = doc.Button("toggle");
        if (GetActive()) but.SetLabel("Pause");
        else but.SetLabel("Start");
      },
      "Start",
      "toggle");

    // Display FPS and current generation of population simulator
    doc << UI::Text("fps") << " FPS = " << UI::Live( [this](){return 1000.0 / GetStepTime();} );
    doc << UI::Text("gen") << " Gen = " << UI::Live( [this](){return pop.gen;} );
  }
  
  void DoFrame()
  {
    // Update the population
    pop.evolve(1);

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
