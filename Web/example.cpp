/*
 *  This file is part of Empirical, https://github.com/devosoft/Empirical
 *  Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  date: 2015-2018
*/
/**
 *  @file
 */

#include "evolution.h"
#include "emp/math/Random.hpp"
#include "emp/base/array.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/canvas_utils.hpp"
#include "emp/web/emfunctions.hpp"
#include "emp/web/web.hpp"

namespace UI = emp::web;

UI::Document doc("emp_base");

emp::Random rng;
double can_size = 600;
int fmap_size = 10;
std::string fitness_map = "../FitnessMaps/10x10_big_vs_small_unequal_peaks.map";
Population p(10000, 0.2, 6, 5);

void DrawPop()
{
  auto mycanvas = doc.Canvas("can");
  double unit = can_size / double(fmap_size); // How far apart units are

  for (int i = 0; i < p.n; ++i)
  {
    double jitterx = rng.GetDouble(-0.4, 0.4);
    double jittery = rng.GetDouble(-0.4, 0.4);
    double x = ((p.first_pop) ? p.pop1[i].x : p.pop2[i].x) + 0.5;
    double y = ((p.first_pop) ? p.pop1[i].y : p.pop2[i].y) + 0.5;
    mycanvas.Circle((x + jitterx) * unit, (y + jittery) * unit, unit / 10.0, "red", "black");
  }
}

void DrawFitnessMap()
{
  auto mycanvas = doc.Canvas("can");
  double unit = can_size / double(fmap_size); // How far apart units are

  std::string color = "grey";

  for (int i = 0; i < fmap_size; ++i)
  {
    for (int j = 0; j < fmap_size; ++j)
    {
      switch(int(p.fitness_map[i][j]))
      {
        case 80:
          color = "blue";
          break;
        case 85:
          color = "purple";
          break;
        case 1:
          color = "grey";
          break;
        default:
          color = "red";
          break;
      }
      mycanvas.Rect(j * unit, i * unit, unit, unit, color, "black");
    }
  }
}

void CanvasAnim(double time) {
  auto mycanvas = doc.Canvas("can");
  mycanvas.Clear();

  std::cerr << time << std::endl;

  // Draw fitness map
  DrawFitnessMap();

  // Update and draw new population
  p.evolve(1, 'r');
  DrawPop();

  doc.Text("fps").Redraw();
  doc.Text("gen").Redraw();
};

int main()
{
  // Canvas width and height
  const double w = can_size;
  const double h = can_size;

  // Create canvas
  auto mycanvas = doc.AddCanvas(w, h, "can");

  // Animation function for the population
  UI::Animate * anim = new UI::Animate(CanvasAnim, mycanvas);
  (void) anim;

  // Load initial fitness function and draw initial population / fitness map
  p.loadFitnessFunction(fitness_map);
  DrawFitnessMap();
  DrawPop();

  // Start button
  doc << "<br>";
  doc.AddButton([anim](){
      anim->ToggleActive();
      auto but = doc.Button("toggle");
      if (anim->GetActive()) but.SetLabel("Pause");
      else but.SetLabel("Start");
    }, "Start", "toggle");

  // FPS counter
  doc << UI::Text("fps") << "FPS = " << UI::Live( [anim](){return anim->GetStepTime()/1000.0;} ) << " ";

  // Generation counter
  doc << UI::Text("gen") << "Gen = " << UI::Live( [](){return p.gen;} ) << " ";
}
