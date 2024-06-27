#ifndef POPULATION_GRAPH_HPP
#define POPULATION_GRAPH_HPP

#include <string>
#include "emp/web/Selector.hpp"
#include "evolution.h"
#include "emp/web/Document.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/TextArea.hpp"

namespace UI = emp::web;

class PopulationGraph : public UI::Animate
{
private:
  // Existing fitness maps to select from
  std::string file1 = "./FitnessMaps/10x10_big_vs_small_unequal_peaks.map";
  std::string file2 = "./FitnessMaps/100x100_big_vs_small_unequal_peaks.map";
  std::string file3 = "./FitnessMaps/100x100_comb.map";
  std::string file4 = "./FitnessMaps/10x10_ring.map";

  double csize; // Size of canvas
  double timeSinceLastEvolve; // Time since last evolution, used for 1 evolve per second mode
  bool fast; // Run animation at fullspeed if true, else 1 fps
  double fitnessEntryValue; // Value stored in fitnessEntryTA, used to update fitness map
  int fitnessSizeEntryValue; // Value stored in fitnessSizeEntryTA, used to update fitness map
  int popEntryValue; // Value stored in popEntryTA, used to update population size
  double mutEntryValue; // Value stored in mutEntryTA, used to update mutation rate
  int startTileX; // Tile X selected by mouse down
  int startTileY; // Tile Y selected by mouse down
  int endTileX; // Tile X selected by mouse up
  int endTileY; // Tile Y selected by mouse up
  
  enum GraphMode
  {
    SIM,
    POP,
    FIT
  };

  GraphMode mode = SIM; // Start in simulation mode

  std::set<std::pair<int, int>> selected; // Selected items for update via mouse click
  std::map<int, std::string> colorMap; // Map of fitness values to colors
  std::vector<std::string> presetColors = // Preset colors used for fitness color map
    {
      "#3B4CC0",
      "#4673CA",
      "#5891D1",
      "#6BAEDA",
      "#82CAE3",
      "#A0E0EF",
      "#C1F1EA",
      "#E6FCF2",
      "#F2E8BF",
      "#F3C68D"
    };
  
  // Key elements
  UI::Document &doc; // Reference to website document
  UI::Canvas fscape; // Canvas to draw on
  UI::Table localLayout; // Track local elements layout
  emp::Random rng; // Random generator for jitter

  // Mode selection
  UI::Button simulationButton; // Select simulation mode
  UI::Button populationButton; // Select population editor
  UI::Button fitnessButton; // Select fitness editor

  // Simulation settings
  UI::Button startButton; // Button for start/pausing evolution
  UI::Button resetButton; // Button for resetting population
  UI::Button speedButton; // Button for changing display speed

  // Editor settings
  UI::Button saveButton; // Save file
  UI::Button loadButton; // Load file
  UI::Selector fitnessMapSelector; // Selector for loading existing fitness maps
  UI::Button fitnessMapButton; // Sets the file based on selector
  UI::Button fitnessClearButton; // Clears selected tiles
  UI::TextArea fitnessEntryTA; // Where to set new values for fitness landscape
  UI::Button fitnessEntryButton; // Enter to set value from fitnessEntryTA
  UI::TextArea fitnessSizeEntryTA; // Where to set new values for fitness landscape size
  UI::Button fitnessSizeEntryButton; // Enter to set value from fitnessSizeEntryTA
  UI::TextArea popEntryTA; // Textbox for updating population size
  UI::Button popEntryButton; // Enter to set value from popEntryTA
  UI::TextArea mutEntryTA; // Textbox for updating mutation rate
  UI::Button mutEntryButton; // Enter to set value from mutEntryTA
  UI::Text fpsText; // Text to display FPS
  UI::Text genText; // Text to display current generation

  // Contains population + fitness map functionality
  Population &pop; // Reference to website population
  
public:
  PopulationGraph(double size, UI::Document &d, Population &p, UI::Table &layout) :
    csize(size),
    timeSinceLastEvolve(0.0),
    fast(true),
    doc(d),
    fscape(csize, csize, "fscape"),
    localLayout(5, 2, "pop_graph_layout"),
    pop(p)
  {
    // Setup population simulator with default fitness map
    pop.loadFitnessFunction(file1);
    CreateColorMap();
    DrawFitnessMap();
    DrawPopulation();

    // Add methods for selecting tiles to manage
    fscape.OnMouseDown( [this](int x, int y) { MouseDown(x, y); } );
    fscape.OnMouseUp( [this](int x, int y) { MouseUp(x, y); } );

    // Button for setting simulator mode
    simulationButton = UI::Button(
      [this]()
      {
        mode = SIM;
        ModeSwap();
        Redraw();
      },
      "Simulation",
      "sim_mode_button"
      );
    simulationButton.SetDisabled();

    // Button for setting population edit mode
    populationButton = UI::Button(
      [this]()
      {
        mode = POP;
        ModeSwap();
        Redraw();
      },
      "Population Editor",
      "pop_mode_button"
      );

    // Button for setting fitness map edit mode
    fitnessButton = UI::Button(
      [this]()
      {
        mode = FIT;
        ModeSwap();
        Redraw();
      },
      "Fitness Editor",
      "fit_mode_button"
      );

    // Button for playing / stopping animation for population simulator
    startButton = UI::Button(
      [this]()
      {
        ToggleActive();
        if (GetActive()) startButton.SetLabel("Pause");
        else startButton.SetLabel("Start");
      },
      "Start",
      "start"
      );

    // Button for restarting
    resetButton = UI::Button(
      [this]()
      {
        pop.reset(6, 5); // Change this so starting pos can be changed
        Redraw();
      },
      "Reset",
      "reset"
      );

    // Button for determining animation mode
    speedButton = UI::Button(
      [this]()
      {
        if (fast) speedButton.SetLabel("Fast");
        else speedButton.SetLabel("Slow");
        fast = !fast;
      },
      "Slow",
      "speed"
      );

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

    // Selector for fitness map selection (pre-existing)
    fitnessMapSelector = UI::Selector("fitness_map_selector");
    fitnessMapSelector.SetOption("File 1");
    fitnessMapSelector.SetOption("File 2");
    fitnessMapSelector.SetOption("File 3");
    fitnessMapSelector.SetOption("File 4");

    fitnessMapButton = UI::Button(
        [this]()
        {
          int id = fitnessMapSelector.GetSelectID();
          switch(id)
          {
          case 0:
            pop.loadFitnessFunction(file1);
            break;
          case 1:
            pop.loadFitnessFunction(file2);
            break;
          case 2:
            pop.loadFitnessFunction(file3);
            break;
          case 3:
            pop.loadFitnessFunction(file4);
            break;
          default:
            pop.loadFitnessFunction(file1);
            break;
          }
          CreateColorMap();
          Redraw();
        },
        "Load Map",
        "fitness_map_button"
      );

    fitnessClearButton = UI::Button(
      [this]()
      {
        selected.clear();
        Redraw();
      },
      "Clear",
      "clear_selected_button"
      );

    fitnessEntryTA = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        fitnessEntryValue = std::strtod(s.c_str(), &end);
        if (end == s.c_str() || *end != '\0' || fitnessEntryValue < 0.0)
        {
          fitnessEntryButton.SetDisabled();
          fitnessEntryTA.SetBorder("1px solid red");
          return;
        }
        fitnessEntryButton.SetDisabled(false);
        fitnessEntryTA.SetBorder("1px solid black");
      },
      "fitness_entry_area"
      );
    fitnessEntryTA.SetSize(80, 20);
    fitnessEntryTA.SetResizableOff();

    fitnessEntryButton = UI::Button(
      [this]()
      {
        for (auto &p : selected)
          pop.fitness_map[p.second][p.first] = fitnessEntryValue;
        CreateColorMap();
        Redraw();
      },
      "Set Value",
      "fitness_entry_button"
      );

    // Text box for entering the size of the fitness map
    fitnessSizeEntryTA = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        fitnessSizeEntryValue = strtol(s.c_str(), &end, 10);
        if (end == s.c_str() || *end != '\0' || fitnessSizeEntryValue < 1 || fitnessSizeEntryValue > MAX_GENE_SIZE)
        {
          fitnessSizeEntryButton.SetDisabled();
          fitnessSizeEntryTA.SetBorder("1px solid red");
          return;
        }
        fitnessSizeEntryButton.SetDisabled(false);
        fitnessSizeEntryTA.SetBorder("1px solid black");
      },
      "fitness_size_entry_area"
      );
    fitnessSizeEntryTA.SetSize(80, 20);
    fitnessSizeEntryTA.SetResizableOff();
    fitnessSizeEntryTA.SetText(std::to_string(pop.xlim));

    fitnessSizeEntryButton = UI::Button(
      [this]()
      {
        // Set all new tiles to 0. If reducing, doesn't matter and will be skipped
        for (int i = 0; i < fitnessSizeEntryValue; ++i)
          for (int j = 0; j < fitnessSizeEntryValue; ++j)
            if (i >= pop.xlim || j >= pop.ylim)
              pop.fitness_map[j][i] = 0.0;
        pop.xlim = fitnessSizeEntryValue;
        pop.ylim = fitnessSizeEntryValue;
        CreateColorMap();
        Redraw();
      },
      "Set Value",
      "fitness_size_entry_button"
      );

    // Text box for setting population
    popEntryTA = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        popEntryValue = std::strtol(s.c_str(), &end, 10);
        if (end == s.c_str() || *end != '\0' || popEntryValue < 1 || popEntryValue > MAX_POP_SIZE)
        {
          popEntryButton.SetDisabled();
          popEntryTA.SetBorder("1px solid red");
          return;
        }
        popEntryButton.SetDisabled(false);    
        popEntryTA.SetBorder("1px solid black");
      },
      "pop_size_entry_area");
    popEntryTA.SetSize(80, 20);
    popEntryTA.SetText(std::to_string(pop.n));
    popEntryTA.SetResizableOff();

    popEntryButton = UI::Button(
      [this]()
      {
        pop.n = popEntryValue;
        Redraw();
      },
      "Set Value",
      "pop_entry_button"
      );

    // Text box for setting mutation rate
    mutEntryTA = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        mutEntryValue = std::strtod(s.c_str(), &end);
        if (end == s.c_str() || *end != '\0' || mutEntryValue < 0.0 || mutEntryValue > 1.0)
        {
          mutEntryButton.SetDisabled();
          mutEntryTA.SetBorder("1px solid red");
          return;
        }
        mutEntryButton.SetDisabled(false);    
        mutEntryTA.SetBorder("1px solid black");
      },
      "mut_rate");
    mutEntryTA.SetSize(80, 20);
    mutEntryTA.SetText(std::to_string(pop.m));
    mutEntryTA.SetResizableOff();

    mutEntryButton = UI::Button(
      [this]()
      {
        pop.m = mutEntryValue;
        Redraw();
      },
      "Set Value",
      "mut_entry_button"
      );

    // Display FPS and current generation of population simulator
    fpsText = UI::Text("fps");
    fpsText << " FPS = " << UI::Live( [this](){return 1000.0 / GetStepTime();} );
    genText = UI::Text("gen");
    genText << " Gen = " << UI::Live( [this](){return pop.gen;} );

    // Set layout
    layout.GetCell(0, 0) << localLayout;
    localLayout.CellsCSS("border", "1px solid black");
    localLayout.CellsCSS("padding", "5px");

    // Mode selection
    localLayout.GetCell(0, 0) << simulationButton << populationButton << fitnessButton;

    // Landscape, main portion
    localLayout.GetCell(1, 0) << fscape;
    
    // Editor label
    localLayout.GetCell(0, 1) << "Simulation Editor";

    // Editor options
    localLayout.GetCell(1, 1)
      << "Population: " << popEntryTA << popEntryButton << "<br>"
      << "Mutation rate: " << mutEntryTA << mutEntryButton << "<br>"
      << "<br><br>"
      << "Existing Maps: " << fitnessMapSelector << fitnessMapButton << "<br>"
      << "Fitness Value: " << fitnessEntryTA << fitnessEntryButton << fitnessClearButton << "<br>"
      << "Map Dimension: " << fitnessSizeEntryTA << fitnessSizeEntryButton << "<br>"
      << "<br><br>"
      << "Save/Load Settings: " << saveButton << loadButton;
    localLayout.GetCell(1, 1).SetCSS("vertical-align", "top");

    // Simulation options
    localLayout.GetCell(2, 0) << startButton << resetButton << speedButton
      << fpsText << genText;

    // Set mode to simulation
    ModeSwap();
  }

  void Redraw()
  {
    // Clear the canvas
    fscape.Clear();

    // Draw background (fitness landscape)
    DrawFitnessMap();

    // Draw updated population if in simulation mode
    if (mode == SIM)
      DrawPopulation();

    // Redraw FPS and Generation
    doc.Text("fps").Redraw();
    doc.Text("gen").Redraw();
  }
  
  void DoFrame()
  {
    // Update the population
    timeSinceLastEvolve += GetStepTime();
    if (fast)
      pop.evolve(1, 'r');
    else if (timeSinceLastEvolve >= 1000)
    {
      pop.evolve(1);
      timeSinceLastEvolve = 0.0;
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
        // Check if tile was selected, display differently if it was
        if (selected.contains(std::pair<int, int>(i, j)))
          fscape.Rect(unit * i, unit * j, unit, unit, "white", "black");
        else // Draw grid tile using assigned color otherwise
          fscape.Rect(unit * i, unit * j, unit, unit, colorMap[pop.fitness_map[j][i]], "black");
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

  // Function to select tile
  void MouseDown(int x, int y)
  {
    if (mode == SIM)
      return;

    double unit = csize / pop.xlim;
    
    // Set start tile
    startTileX = (x / unit);
    startTileY = (y / unit);
  }

  // Function to select tile
  void MouseUp(int x, int y)
  {
    if (mode == SIM)
      return;

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

    Redraw();
  }

  // Function to create color map
  void CreateColorMap()
  {
    // Reset color mapping
    colorMap.clear();

    // Add all fitness levels (std::map will auto sort)
    for (int i = 0; i < pop.xlim; ++i)
    {
      for (int j = 0; j < pop.ylim; ++j)
      {
        colorMap.insert(std::pair<int, std::string>(pop.fitness_map[j][i], "black"));
      }
    }
    
    // If there are no more than 10 fitness levels outside of 0, use preset colors
    if (colorMap.size() <= 10 || (colorMap.size() == 11 && colorMap.count(0)))
    {
      int i = 0;
      for (auto it = colorMap.begin(); it != colorMap.end(); ++it)
      {
        // Zero gets set to black
        if (it->first == 0)
        {
          it->second = "black";
          continue;
        }

        it->second = presetColors[i];
        ++i;
      }
    }
    else // Map colors as needed
    {
      // !!! DO STUFF HERE !!!
      std::cout << "More colors for colormap needs implemented!" << std::endl;
      std::cout << "Color map size: " << colorMap.size() << std::endl;
    }
  }

  // Function to handle mode swapping, such as enabling/disabling elements, pausing simulation, etc
  // Simulation mode determined by mode variable, set that to SIM, POP, FIT, then call this
  void ModeSwap()
  {
    bool sim = mode == SIM;
    bool pop = mode == POP;
    bool fit = mode == FIT;
      
    // Stop simulation when swapping modes
    if (GetActive())
    {
      ToggleActive();
      startButton.SetLabel("Start");
    }

    // Mode selection buttons
    simulationButton.SetDisabled(sim);
    populationButton.SetDisabled(pop);
    fitnessButton.SetDisabled(fit);

    // Simulation elements
    startButton.SetDisabled(!sim);
    resetButton.SetDisabled(!sim);
    speedButton.SetDisabled(!sim);

    // Population editor elements
    popEntryButton.SetDisabled(!pop);
    popEntryTA.SetDisabled(!pop);
    mutEntryButton.SetDisabled(!pop);
    mutEntryTA.SetDisabled(!pop);

    // Fitness map editor elements
    fitnessMapSelector.Disabled(!fit); // Different naming scheme for whatever reason, does same as rest
    fitnessMapButton.SetDisabled(!fit);
    fitnessEntryButton.SetDisabled(!fit);
    fitnessEntryTA.SetDisabled(!fit);
    fitnessClearButton.SetDisabled(!fit);
    fitnessSizeEntryButton.SetDisabled(!fit);
    fitnessSizeEntryTA.SetDisabled(!fit);

    // If mode is not fitness editor
    if (!fit)
    {
      selected.clear();
    }
  }
};

#endif
