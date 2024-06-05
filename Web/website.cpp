#include "website.h"
#include "emp/base/array.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/web/Document.hpp"
#include "evolution.h"

void Test(double time) {}

// Website constructor
Website::Website() :
  csize(500.0),
  doc("emp_base"),
  fscape(doc.AddCanvas(csize, csize, "fscape")),
  fmap(1, 1, "fmap"),
  fanim(Test),
  p(10000, 0.1)
{
  // Setup population simulator
  p.loadFitnessFunction("../FitnessMaps/10x10_big_vs_small_unequal_peaks.map");
  DrawFitnessMap();
  DrawPopulation();

  // Button for playing / stopping animation for population simulator
  doc << "<br>";
  doc.AddButton([this](){
      fanim.ToggleActive();
      auto but = doc.Button("toggle");
      if (fanim.GetActive()) but.SetLabel("Pause");
      else but.SetLabel("Start");
    }, "Start", "toggle");

  // Display FPS and current generation of population simulator
  doc << UI::Text("fps") << "FPS = " << UI::Live( [this](){return 1000.0 / fanim.GetStepTime();} );
  doc << UI::Text("gen") << "Gen = " << UI::Live( [this](){return p.gen;} );

  // Setup fitness map editor / visualizer
  //doc << fmap;
}

// Function to animate the fitness landscape
void Website::FitnessAnim(double time)
{
  /*
  // Clear the canvas
  fscape.Clear();

  // Draw the current fitness map as background
  DrawFitnessMap();

  // Simulate a generation, then draw population
  p.evolve(1);
  DrawPopulation();

  // Redraw fps and current generation
  doc.Text("fps").Redraw();
  doc.Text("gen").Redraw();
  */
}

// Function that draws the fitness landscape as a background
void Website::DrawFitnessMap()
{
  double unit = csize / p.xlim;
  std::string color;

  for (int i = 0; i < p.xlim; ++i)
  {
    for (int j = 0; j < p.ylim; ++j)
    {
      // Choose tile color based on fitness
      switch(int(p.fitness_map[j][i]))
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
void Website::DrawPopulation()
{
  double unit = csize / p.xlim;

  for (int i = 0; i < p.n; ++i)
  {
    // Add jitter
    double xjitter = rng.GetDouble(-0.4, 0.4);
    double yjitter = rng.GetDouble(-0.4, 0.4);
    
    // Choose x and y from current population, +0.5 to center on squares
    double x = ((p.first_pop) ? p.pop1[i].x : p.pop2[i].x) + 0.5;
    double y = ((p.first_pop) ? p.pop1[i].y : p.pop2[i].y) + 0.5;

    // Draw circles scaled to unit size for each organism
    fscape.Circle((x + xjitter) * unit, (y + yjitter) * unit, unit / 10.0, "red", "black");
  }
}

int main()
{
  Website w;
}
