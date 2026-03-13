#pragma once

#include "Puzzle.h"
#include "vector.h"

/**
 * updateDisplay
 * ------------
 * updateDisplay is the only function from this module that you will need to call
 * in your code. You pass a Puzzle and a Vector of remaining tiles and this will
 * show the current state of the grid and remaining tiles on the graphical display.
 * The optional third argument is a count of milliseconds to pause after updating
 * the display. If not given, there is no pause.
 */
void updateDisplay(const Puzzle& puzzle, const Vector<Tile>& tiles, int pauseMs=0);


/**
 * chooseFileDialog
 * ----------------
 * Brings up a file chooser dialog allowing the user to choose a puzzle
 * configuration file. Returns the name of the file chosen, or empty
 * string if the user canceled.
 */
std::string chooseFileDialog();

/**
 * loadPuzzleConfig
 * ----------------
 * Reads the puzzle configuration from the specified file and populates the
 * an empty puzzle and vector of remaining tiles. Returns true if load was
 * successful, false on any error.
 */
bool loadPuzzleConfig(std::string configFile, Puzzle& puzzle, Vector<Tile>& tiles);

enum Action { NONE, RUN_SOLVE, LOAD_NEW, QUIT };

/**
 * playInteractive
 * ----------------
 * Displays the current state of puzzle and remaining tiles and goes into a
 * loop which allows the user to interactively control the puzzle (select tiles,
 * rotate, add, remove). The function does not exit the loop until the user
 * either quits (window close box), or requests load of new puzzle or
 * run computer solver. The function returns which action exited the loop.
 */
Action playInteractive(Puzzle& puzzle, Vector<Tile>& tiles);

