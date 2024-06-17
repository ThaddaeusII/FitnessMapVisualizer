#ifndef Simulation_EDITOR_HPP
#define Simulation_EDITOR_HPP

#include "evolution.h"
#include "emp/web/Canvas.hpp"
#include "emp/web/Document.hpp"
#include "emp/web/Table.hpp"

namespace UI = emp::web;

class SimulationEditor
{
private:
  double csize; // Size of canvas
  double entryValue; // Value stored in entryArea, used to update fitness map
  int startTileX; // Tile X selected by mouse down
  int startTileY; // Tile Y selected by mouse down
  int endTileX; // Tile X selected by mouse up
  int endTileY; // Tile Y selected by mouse up
  std::set<std::pair<int, int>> selected; // Selected items for update

  UI::Document &doc; // Document from website class
  Population &pop; // Population from website class
  UI::Table localLayout; // Manages local layout of the editor
  UI::Canvas canvas; // Canvas to dray simulator on
  UI::Button populationButton; // Select population editor
  UI::Button fitnessButton; // Select fitness editor
  UI::Button saveButton; // Save file
  UI::Button loadButton; // Load file
  UI::Button entryButton; // Enter to set value from entryArea
  UI::Button clearButton; // Clears selected tiles
  UI::TextArea entryArea; // Where to set new value for fitness

public:
  SimulationEditor(double csize, UI::Document &d, Population &p, UI::Table &layout) :
    csize(csize),
    doc(d),
    pop(p),
    localLayout(3, 1, "editor_layout"),
    canvas(csize, csize, "editor_canvas")
  {
    // Add methods for selecting tiles to manage
    canvas.OnMouseDown( [this](int x, int y) { MouseDown(x, y); } );
    canvas.OnMouseUp( [this](int x, int y) { MouseUp(x, y); } );

    populationButton = UI::Button(
      [this]()
      {
        //
      },
      "Population Editor",
      "pop_mode_button"
      );
    populationButton.SetDisabled();

    fitnessButton = UI::Button(
      [this]()
      {
        //
      },
      "Fitness Editor",
      "fit_mode_button"
      );
    fitnessButton.SetDisabled();

    saveButton = UI::Button(
      [this]()
      {
        //
      },
      "Save",
      "save_edit_to_file_button"
      );
    saveButton.SetDisabled();

    loadButton = UI::Button(
      [this]()
      {
        //
      },
      "Load",
      "load_file_to_edit_button"
      );
    loadButton.SetDisabled();

    entryButton = UI::Button(
      [this]()
      {
        for (auto &p : selected)
          pop.fitness_map[p.second][p.first] = entryValue;
        DrawSimulationMap();
      },
      "Set Value",
      "set_entry_button"
      );

    clearButton = UI::Button(
      [this]()
      {
        selected.clear();
        DrawSimulationMap();
      },
      "Clear",
      "clear_selected_button"
      );

    entryArea = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        entryValue = std::strtod(s.c_str(), &end);
        if (end == s.c_str() || *end != '\0')
        {
          entryButton.SetDisabled();
          return;
        }
        entryButton.SetDisabled(false);
      },
      "editor_entry_area"
      );
    entryArea.SetSize(80, 20);

    // Set layout
    layout.GetCell(0, 1) << localLayout;
    localLayout.GetCell(0, 0) << populationButton << fitnessButton;
    localLayout.GetCell(1, 0) << canvas;
    localLayout.GetCell(2, 0) << entryArea << entryButton << clearButton << saveButton << loadButton;
    localLayout.CellsCSS("border", "1px solid black");
    localLayout.CellsCSS("padding", "5px");
    
    // Initial drawing
    DrawSimulationMap();
  }

  // Function to select tile
  void MouseDown(int x, int y)
  {
    std::cout << x << " vs " << canvas.GetXPos() << std::endl;
    std::cout << y << " vs " << canvas.GetYPos() << std::endl;

    double unit = csize / pop.xlim;
    
    // Set start tile
    startTileX = (x / unit);
    startTileY = (y / unit);
  }

  // Function to select tile
  void MouseUp(int x, int y)
  {
    double unit = csize / pop.xlim;
    
    // Set end tile
    endTileX = (x / unit);
    endTileY = (y / unit);

    // Select range
    for (int i = std::min(startTileX, endTileX); i < std::max(startTileX, endTileX) + 1; ++i)
    {
      for (int j = std::min(startTileY, endTileY); j < std::max(startTileY, endTileY) + 1; ++j)
      {
        auto it = selected.find(std::pair<int, int>(i, j));
        if (it != selected.end())
        {
          selected.erase(it);
          continue;
        }
        selected.insert(std::pair<int, int>(i, j));
      }
    }

    DrawSimulationMap();
  }

  // Function that draws the fitness landscape as a background
  void DrawSimulationMap()
  {
    canvas.Clear();

    double unit = csize / pop.xlim;
    std::string color;

    for (int i = 0; i < pop.xlim; ++i)
    {
      for (int j = 0; j < pop.ylim; ++j)
      {
        // Check if tile was selected, display red if it was
        std::string color = "grey";
        if (selected.contains(std::pair<int, int>(i, j)))
          color = "red";

        // Draw grid tile using assigned color
        canvas.Rect(unit * i, unit * j, unit, unit, color, "black");
      }
    }
  }
};

#endif
