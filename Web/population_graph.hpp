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
  int fitnessSizeEntryValue; // Value stored in fitnessSizeEntryTA, used to update fitness map
  double fitnessTileEntryValue; // Value stored in fitnessTileEntryTA, used to add new fitness level to color map
  int popEntryValue; // Value stored in popEntryTA, used to update population size
  double mutEntryValue; // Value stored in mutEntryTA, used to update mutation rate
  int startTileX; // Tile X selected by mouse down
  int startTileY; // Tile Y selected by mouse down
  int endTileX; // Tile X selected by mouse up
  int endTileY; // Tile Y selected by mouse up
  int colorSelected = -1; // Selected color on the map
  bool drawingOn = false; // Bool for if currently able to draw the fitness levels
  double selectedColorFitness; // fitness associated with current color selected, for drawing
  std::string colorHexValue; // Value of currently entered hex
  int colorFitnessValue; // Value of fitness being assigned to a color
  bool colorHexValid; // Check for if the input to colorHexValue is valid
  bool colorFitnessValid; // Check for if the input to colorHexValue is valid
  std::string selectionMethod = "r"; // Selection method parameter: "r" = roulette, "t" = tournament
  int tournamentSize = 7; // Tournament size
  int tournamentSizeEntry; // Tournament size in the entry area, used to update tournamentSize

  // Modes that the simulator can be in, Simulation, Population Editor, and Fitness Editor
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
  UI::Table colorMapLayout; // Table to arrange colors
  emp::Random rng; // Random generator for jitter
  emp::vector<UI::Canvas> colorMapTiles; // Tiles for displaying color map

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
  UI::Button distributePopulationButton; // Button for uniformly distributing population among selected tiles
  UI::Button clearSelectedButton; // Clears selected population
  UI::Selector selectionMethodSelector; // Options for selection method
  UI::Button selectionMethodButton; // Button to set selection method
  UI::Selector fitnessMapSelector; // Selector for loading existing fitness maps
  UI::Button fitnessMapButton; // Sets the file based on selector
  UI::TextArea fitnessSizeEntryTA; // Where to set new values for fitness landscape size
  UI::Button fitnessSizeEntryButton; // Enter to set value from fitnessSizeEntryTA
  UI::TextArea fitnessTileEntryTA; // Where to set new values for fitness landscape tile
  UI::Button fitnessTileEntryButton; // Enter to add new fitness level to color map
  UI::TextArea setColorHexTA; // Entry area for changing color map colors
  UI::Button setColorHexButton; // Button for setting color from setColorHexTA
  UI::TextArea setColorFitnessTA; // Entry area for changing color fitness value
  UI::Button setColorFitnessButton; // Button for setting fitness from setColorFitnessTA
  UI::Button removeColorButton; // Button for removing a color
  UI::TextArea popEntryTA; // Textbox for updating population size
  UI::Button popEntryButton; // Enter to set value from popEntryTA
  UI::TextArea mutEntryTA; // Textbox for updating mutation rate
  UI::Button mutEntryButton; // Enter to set value from mutEntryTA
  UI::TextArea tournamentSizeTA; // Text area for entering tournament size
  UI::Button tournamentSizeButton; // Button for setting tournament size from tournamentSizeTA
  UI::Text fpsText; // Text to display FPS
  UI::Text genText; // Text to display current generation
  UI::Text selectionText; // Text to display selection type

  // Contains population + fitness map functionality
  Population &pop; // Reference to website population
  
public:
  PopulationGraph(double size, UI::Document &d, Population &p, UI::Table &layout) :
    csize(size),
    timeSinceLastEvolve(0.0),
    fast(true),
    doc(d),
    fscape(csize, csize, "fscape"),
    localLayout(3, 3, "pop_graph_layout"),
    colorMapLayout(1, 1, "col_map_layout"),
    pop(p)
  {
    // Setup population simulator with default fitness map
    pop.loadFitnessFunction(file1);
    CreateColorMap();
    DrawFitnessMap();
    DrawPopulation();
    DrawColorMap();

    // Add methods for selecting tiles to manage
    fscape.OnMouseDown( [this](int x, int y) { MouseDown(x, y); } );
    fscape.OnMouseUp( [this](int x, int y) { MouseUp(x, y); } );
    fscape.OnMouseMove( [this](int x, int y) { MouseMove(x, y); } );
      
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
        pop.reset();
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

    // Button for distributing population
    distributePopulationButton = UI::Button(
      [this]()
      {
        auto itr = selected.begin();
        for (int i = 0; i < pop.n; ++i)
        {
          // Determine which population storage is in use
          if (pop.first_pop)
          {
            pop.pop1[i].x = itr->first;
            pop.pop1[i].y = itr->second;
          }
          else
          {
            pop.pop2[i].x = itr->first;
            pop.pop2[i].y = itr->second;
          }

          // Cycle through selected tiles for distribution
          itr++;
          if (itr == selected.end())
            itr = selected.begin();
        }

        // Save initial population so reset brings it here
        pop.newInitPop();

        // Clear selected and disable editor elements
        selected.clear();
        distributePopulationButton.SetDisabled();
        clearSelectedButton.SetDisabled();

        // Redraw when done
        Redraw();
      },
      "Distribute",
      "distribute_population_button"
      );
    distributePopulationButton.SetDisabled();

    // Button for clearing selected tiles for distribution
    clearSelectedButton = UI::Button(
      [this]()
      {
        selected.clear();
        distributePopulationButton.SetDisabled();
        clearSelectedButton.SetDisabled();
        Redraw();
      },
      "Clear",
      "clear_selected_button"
      );
    clearSelectedButton.SetDisabled();

    // Selector for selection method
    selectionMethodSelector = UI::Selector("selection_method_selector");
    selectionMethodSelector.SetOption("Roulette");
    selectionMethodSelector.SetOption("Tournament");

    // Used to set option from selectionMethodSelector
    selectionMethodButton = UI::Button(
      [this]()
      {
        selectionText.Clear();
        int id = selectionMethodSelector.GetSelectID();
        switch(id)
        {
        case 0:
          selectionMethod = 'r';
          selectionText << "Roulette" << "<br>";
          break;
        case 1:
          selectionMethod = 't';
          selectionText << "Tournament (" << tournamentSize << ")" << "<br>";
          break;
        default:
          selectionMethod = 'r';
          selectionText << "Roulette" << "<br>";
          break;
        }
      },
      "Select",
      "selection_method_button"
      );

    // Text area for entering tournament size
    tournamentSizeTA = UI::TextArea(
      [this](const std::string& s)
      {
        char* end;
        tournamentSizeEntry = strtol(s.c_str(), &end, 10);
        if (end == s.c_str() || *end != '\0' || tournamentSizeEntry < 1 || tournamentSizeEntry > pop.n)
        {
          tournamentSizeButton.SetDisabled();
          tournamentSizeTA.SetBorder("1px solid red");
          return;
        }
        tournamentSizeButton.SetDisabled(false);
        tournamentSizeTA.SetBorder("1px solid black");
      },
      "tournement_entry_area"
      );
    tournamentSizeTA.SetSize(80, 20);
    tournamentSizeTA.SetResizableOff();
    tournamentSizeTA.SetText(std::to_string(tournamentSize));

    // Button for setting tournament size
    tournamentSizeButton = UI::Button(
      [this]()
      {
        // Update tournament size and the display
        tournamentSize = tournamentSizeEntry;
        doc.Text("selection").Redraw();
      },
      "Set Size",
      "tournement_entry_button"
      );

    // Selector for fitness map selection (pre-existing)
    fitnessMapSelector = UI::Selector("fitness_map_selector");
    fitnessMapSelector.SetOption("File 1");
    fitnessMapSelector.SetOption("File 2");
    fitnessMapSelector.SetOption("File 3");
    fitnessMapSelector.SetOption("File 4");

    // Used to set option from fitnessMapSelector
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

    // Text box for entering the size of the fitness map
    fitnessTileEntryTA = UI::TextArea(
      [this](const std::string &s)
      {
        char* end;
        fitnessTileEntryValue = strtod(s.c_str(), &end);
        if (end == s.c_str() || *end != '\0' || fitnessTileEntryValue < 1 || fitnessTileEntryValue > MAX_GENE_SIZE)
        {
          fitnessTileEntryButton.SetDisabled();
          fitnessTileEntryTA.SetBorder("1px solid red");
          return;
        }
        fitnessTileEntryButton.SetDisabled(false);
        fitnessTileEntryTA.SetBorder("1px solid black");
      },
      "fitness_tile_entry_area"
      );
    fitnessTileEntryTA.SetSize(80, 20);
    fitnessTileEntryTA.SetResizableOff();

    fitnessTileEntryButton = UI::Button(
      [this]()
      {
        // Add a new color and redraw, with fitness from fitnessTileEntryTA
        colorMap.insert(std::pair<int, std::string>(fitnessTileEntryValue, "black"));
        DrawColorMap();
      },
      "Add Fitness Level",
      "fitness_tile_entry_button"
      );

    // Button for entering new color (hex)
    setColorHexTA = UI::TextArea(
      [this](const std::string &s)
      {
        // Ensure hex has correct size and starts with #
        if (s.size() != 7 || s[0] != '#')
        {
          setColorHexButton.SetDisabled();
          colorHexValue = false;
          setColorHexTA.SetCSS("border", "1px solid red");
          return;
        }

        // Ensure all characters are 0-9,A-F
        for (int i = 1; i < 7; ++i)
        {
          if ( !(s[i] >= 'A' && s[i] <= 'F') && !(s[i] >= '0' && s[i] <= '9') )
          {
            setColorHexButton.SetDisabled();
            colorHexValue = false;
            setColorHexTA.SetCSS("border", "1px solid red");
            return;
          }
        }
        // Enable if value is valid and there is a selected color to change
        colorHexValid = true;
        if (colorSelected != -1)
          setColorHexButton.SetDisabled(false);

        setColorHexTA.SetCSS("border", "1px solid black");
        colorHexValue = s;
      },
      "color_entry_text_area"
      );
    setColorHexTA.SetSize(80, 20);
    setColorHexTA.SetResizableOff();
    setColorHexTA.SetText("#______");

    // Button for setting color from the color setting text area
    setColorHexButton = UI::Button(
      [this]()
      {
        colorMap[selectedColorFitness] = colorHexValue;
        colorSelected = -1;
        selectedColorFitness = -1;
        DrawColorMap();
        Redraw();
      },
      "Change Color",
      "color_entry_button"
      );
    setColorHexButton.SetDisabled();

    // Button for changing fitness of a color
    setColorFitnessTA = UI::TextArea(
      [this](const std::string &s)
      {
        char *end;
        colorFitnessValue = strtol(s.c_str(), &end, 10);
        if (end == s.c_str() || *end != '\0' || colorFitnessValue < 0 || colorMap.count(colorFitnessValue) > 0)
        {
          setColorFitnessButton.SetDisabled();
          setColorFitnessTA.SetBorder("1px solid red");
          return;
        }
        
        // Enable if value is valid and there is a selected color to change
        colorFitnessValid = true;
        if (colorSelected != -1)
          setColorFitnessButton.SetDisabled(false);

        setColorFitnessTA.SetCSS("border", "1px solid black");
      },
      "color_fitness_entry_text_area"
      );
    setColorFitnessTA.SetSize(80, 20);
    setColorFitnessTA.SetResizableOff();

    // Button for setting color from the color setting text area
    setColorFitnessButton = UI::Button(
      [this]()
      {
        // Set all instances of selected color to colorFitnessValue
        for (int i = 0; i < pop.xlim; ++i)
          for (int j = 0; j < pop.ylim; ++j)
            if (pop.fitness_map[j][i] == selectedColorFitness)
              pop.fitness_map[j][i] = colorFitnessValue;
        
        // Swap color map entry to use new fitness value for the color
        colorMap[colorFitnessValue] = colorMap[selectedColorFitness];
        colorMap.erase(selectedColorFitness);

        colorSelected = -1;
        selectedColorFitness = -1;
        DrawColorMap();
      },
      "Change fitness",
      "color_fitness_entry_button"
      );
    setColorFitnessButton.SetDisabled();
    
    // Button for removing color, should not be able to remove last color
    removeColorButton = UI::Button(
      [this]()
      {
        // Don't remove last color
        if (colorMap.size() == 1)
          return;

        // Change all instances of fitness being removed to 0
        for (int i = 0; i < pop.xlim; ++i)
        {
          for (int j = 0; j < pop.ylim; ++j)
          {
            if (pop.fitness_map[j][i] == selectedColorFitness)
            {
              pop.fitness_map[j][i] = 0;
            }
          }
        }

        // Remove color (can't delete 0/"black", as it is default and will be added back)
        colorMap.erase(selectedColorFitness);

        // If 0 isn't in color map, add it as black default
        if (colorMap.count(0) == 0)
        {
          colorMap[0] = "black";
        }

        colorSelected = -1;
        selectedColorFitness = -1;
        DrawColorMap();
        Redraw();
      },
      "Remove Color",
      "remove_color_button"
      );
    removeColorButton.SetDisabled();

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
    selectionText = UI::Text("selection");
    selectionText << "Selection = " << UI::Live( [this](){return tournamentSize;} );

    // Set layout
    layout.GetCell(0, 0) << localLayout;
    localLayout.CellsCSS("border", "1px solid black");
    localLayout.CellsCSS("padding", "5px");

    // Mode selection
    localLayout.GetCell(0, 0) << simulationButton << populationButton << fitnessButton;

    // Landscape, main portion
    localLayout.GetCell(1, 0) << fscape;
    
    // Color map label
    localLayout.GetCell(0, 1) << "Color Map";

    // Color map
    localLayout.GetCell(1, 1) << colorMapLayout;
    colorMapLayout.CellsCSS("border", "1px solid black");
    colorMapLayout.CellsCSS("padding", "5px");
    localLayout.GetCell(1, 1).SetCSS("vertical-align", "top");

    // Editor label
    localLayout.GetCell(0, 2) << "Simulation Editor";
    
    // Editor options are set in ModeSwap()
    // localLayout.GetCell(1, 2);

    // Simulation options
    localLayout.GetCell(2, 0) << startButton << resetButton << speedButton
      << selectionText << "<br>" << fpsText << genText;

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
    if (mode != FIT)
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
      pop.evolve(1, selectionText, tournamentSize);
    else if (timeSinceLastEvolve >= 1000)
    {
      pop.evolve(1, selectionText, tournamentSize);
      timeSinceLastEvolve = 0.0;
    }

    Redraw();
  }
  
  // Function that draws the fitness landscape as a background
  void DrawFitnessMap()
  {
    double unit = csize / pop.xlim;
    std::string border;

    for (int i = 0; i < pop.xlim; ++i)
    {
      for (int j = 0; j < pop.ylim; ++j)
      {
        border = "black";

        // Check if tile was selected, display differently if it was
        if (selected.contains(std::pair<int, int>(i, j)))
          border = "red";

        fscape.Rect(unit * i, unit * j, unit, unit, colorMap[pop.fitness_map[j][i]], border);
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
      double xjitter = rng.GetDouble(-0.35, 0.35);
      double yjitter = rng.GetDouble(-0.35, 0.35);
    
      // Choose x and y from current population, +0.5 to center on squares
      double x = ((pop.first_pop) ? pop.pop1[i].x : pop.pop2[i].x) + 0.5;
      double y = ((pop.first_pop) ? pop.pop1[i].y : pop.pop2[i].y) + 0.5;

      // Draw circles scaled to unit size for each organism
      fscape.Circle((x + xjitter) * unit, (y + yjitter) * unit, unit / 10.0, "red", "black");
    }
  }

  // Draw the color map
  void DrawColorMap()
  {
    // Clear the color map table and resize as needed
    colorMapLayout.ClearCells();
    colorMapLayout.Resize((colorMap.size() < 10) ? colorMap.size() % 10 : 10, (colorMap.size() / 10 + 1) * 2);
    colorMapLayout.CellsCSS("border", "1px solid black");
    colorMapLayout.CellsCSS("padding", "5px");

    // Add new tile canvases if needed
    while (colorMap.size() > colorMapTiles.size())
      colorMapTiles.push_back(UI::Canvas(40, 40));

    // Display all colors vs fitness in table
    int i = 0;
    for (auto &c : colorMap)
    {
      colorMapTiles[i].Clear();
      if (colorSelected != i)
        colorMapTiles[i].Rect(0, 0, 40, 40, c.second, "black");
      else
      {
        colorMapTiles[i].Rect(0, 0, 40, 40, "red", "black");
        colorMapTiles[i].Rect(3, 3, 34, 34, c.second, "black");
      }
      colorMapTiles[i].OnMouseDown(
        [this, c, i](int x, int y)
        {
          if (mode != FIT)
            return;

          // If this color was selected already, reset
          if (colorSelected == i)
          {
            colorSelected = -1;
            selectedColorFitness = -1;

            // Disable editing features
            setColorHexButton.SetDisabled();
            setColorFitnessButton.SetDisabled();
            removeColorButton.SetDisabled();

            DrawColorMap();
            return;
          }

          // Otherwise, highlight border
          colorSelected = i;
          selectedColorFitness = c.first;

          // Enable editing options
          if (colorHexValid)
            setColorHexButton.SetDisabled(false);
          if (colorFitnessValid)
            setColorFitnessButton.SetDisabled(false);
          if (colorMap.size() > 1)
            removeColorButton.SetDisabled(false);

          DrawColorMap();
        }
        );
      colorMapLayout.GetCell(i % 10, (i / 10) * 2) << colorMapTiles[i];
      colorMapLayout.GetCell(i % 10, (i / 10) * 2 + 1) << c.first;
      i++;
    }
  }

  // Function to select tile
  void MouseDown(int x, int y)
  {
    if (mode == POP)
    {
      double unit = csize / pop.xlim;
    
      // Set start tile
      startTileX = (x / unit);
      startTileY = (y / unit);
    }
    else if (mode == FIT)
    {
      drawingOn = true;
      
      if (colorSelected >= 0)
      {
        // Check the click location for drawing, same as MouseMove
        double unit = csize / pop.xlim;
        int tileX = (x / unit);
        int tileY = (y / unit);

        // Color tile with new color
        if (pop.fitness_map[tileY][tileX] != selectedColorFitness)
        {
          pop.fitness_map[tileY][tileX] = selectedColorFitness;
          Redraw();
        }
      }
    }
  }

  // Function to select tile
  void MouseUp(int x, int y)
  {
    if (mode == POP)
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

      // Check whether to enable or disable editing tools
      if (selected.size() == 0)
      {
        distributePopulationButton.SetDisabled();
        clearSelectedButton.SetDisabled();
      }
      else
      {
        distributePopulationButton.SetDisabled(false);
        clearSelectedButton.SetDisabled(false);
      }

      // Redraw with tiles selected highlighted
      Redraw();
    }
    else if (mode == FIT)
    {
      drawingOn = false;
    }
  }

  void MouseMove(int x, int y)
  {
    if (mode == FIT && drawingOn && colorSelected >= 0)
    {
      double unit = csize / pop.xlim;
      int tileX = (x / unit);
      int tileY = (y / unit);

      // Color tile with new color
      if (pop.fitness_map[tileY][tileX] != selectedColorFitness)
      {
        pop.fitness_map[tileY][tileX] = selectedColorFitness;
        Redraw();
      }
    }
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

    // Redraw the color map
    DrawColorMap();
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

    // Clear editor table, add general settings
    localLayout.ClearCell(1, 2);
    localLayout.GetCell(1, 2).SetCSS("border", "1px solid black");
    localLayout.GetCell(1, 2).SetCSS("padding", "5px");
    localLayout.GetCell(1, 2).SetCSS("vertical-align", "top");

    // Mode selection buttons
    simulationButton.SetDisabled(sim);
    populationButton.SetDisabled(pop);
    fitnessButton.SetDisabled(fit);

    // Simulation elements
    startButton.SetDisabled(!sim);
    resetButton.SetDisabled(!sim);
    speedButton.SetDisabled(!sim);

    // Simulation editor elements
    if (sim)
    {
      localLayout.GetCell(1, 2)
        << selectionMethodSelector << selectionMethodButton
        << "<br>" << "Tournament size: " << tournamentSizeTA << tournamentSizeButton;
    }

    // Population editor elements
    if (pop)
    {
      localLayout.GetCell(1, 2)
        << "Population: " << popEntryTA << popEntryButton << "<br>"
        << "Mutation rate: " << mutEntryTA << mutEntryButton << "<br>"
        << "Distrubte population: " << distributePopulationButton << "<br>"
        << "Clear selected: " << clearSelectedButton << "<br>"
        << "<br><br>"
        << "Save/Load Settings: " << saveButton << loadButton;
    }

    // Fitness map editor elements
    if (fit)
    {
      localLayout.GetCell(1, 2)
        << "Existing Maps: " << fitnessMapSelector << fitnessMapButton << "<br>"
        << "Map Dimension: " << fitnessSizeEntryTA << fitnessSizeEntryButton << "<br>"
        << "Add new fitness level: " << fitnessTileEntryTA << fitnessTileEntryButton << "<br>"
        << "Change color: " << setColorHexTA << setColorHexButton << "<br>"
        << "Change fitness: " << setColorFitnessTA << setColorFitnessButton << "<br>"
        << "Remove color: " << removeColorButton << "<br>"
        << "<br><br>"
        << "Save/Load Settings: " << saveButton << loadButton;
    }

    // Clear any selected elements
    selected.clear();

    // Draw color map to remove any selections / update changes
    DrawColorMap();
  }
};

#endif
