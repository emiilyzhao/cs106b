/*
 * This file contains the implementation of the solving of edge-matching puzzles. For these puzzles
 * we are given square tiles that have uniquely patterned edges. We are tasked with placing all
 * tiles into a grid, having located and rotated all tiles in a way so that all tiles "match"
 *
 */

#include "puzzle-solve.h"
#include "Puzzle.h"
#include "PuzzleGUI.h"
#include "SimpleTest.h"

using namespace std;

void tileMatch() {
    Puzzle puzzle;
    Vector<Tile> tiles;
    Action action;

    loadPuzzleConfig("puzzles/turtles/turtles.txt", puzzle, tiles);
    updateDisplay(puzzle, tiles);

    do {
        action = playInteractive(puzzle, tiles);
        if (action == LOAD_NEW) {
            string configFile = chooseFileDialog();
            loadPuzzleConfig(configFile, puzzle, tiles);
            updateDisplay(puzzle, tiles);
        } else if (action == RUN_SOLVE) {
            bool success = solve(puzzle, tiles);
            cout << "Found solution to puzzle? " << boolalpha << success << endl;
            updateDisplay(puzzle, tiles);
        }
    } while (action != QUIT);
}

// This function solves the puzzle through recursion. Taking a puzzle and all potential tiles,
// we iterate through all available tiles, checking if they are valid in all directions.
// For each unique orientation, we check the remaining tiles that are left. If no possible
// combinations are left and the puzzle is incomplete, false is returned. We remove any tiles
// that produce no possible further combinations. if the puzzle is full
// and complete, true is returned. Every time the tile is added to the puzzle, it is removed from
// the potential remaining tiles, and vise versa when it is removed.
bool solve(Puzzle& puzzle, Vector<Tile>& tileVec) {
    // base case: puzzle full (successful)
    if (puzzle.isFull())
        return true;

    // base case: still space but no tiles left to place (unsuccessful)
    if (tileVec.isEmpty())
        return false;

    // iterate through all available tiles
    for (int i = 0; i < tileVec.size(); i++) {
        Tile tile  = tileVec[i];
        Tile holder = tileVec.remove(i);

        // check validity in all directions
        for (int rot = 0; rot < 4; rot++) {

            // if valid: add into grid, remove from vector, continue iterating
            if (puzzle.canAdd(tile)) {
                puzzle.add(tile);
                updateDisplay(puzzle, tileVec, 5);

                // keep trying to solve from here
                if (solve(puzzle, tileVec))
                    return true;

                // if doesn't keep working, add back in
                puzzle.remove();
                updateDisplay(puzzle, tileVec, 5);
            }

            // try next rotation
            tile.rotate();
        }

        // none of the rotations work, put tile back in
        tileVec.insert(i, holder);
    }

    // nothing works, so lets remove this from grid and add back into vector
    return false;
}
