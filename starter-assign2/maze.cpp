/* This file correspondes with "Mazes" in assignemnt 2, containing functions generateValidMoves,
 * validatePath, solveMazeBFS, solveMazeDFS, readMazeFileName, and readResolutionFile,
 * along with corresponding provided and student test functions.
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// This function takes two parameters Grid maze and GridLocation cur, and returns a set of
// GridLocations that correspond with valid moves from cur. The function looks at GridLocations
// that are north, east, south, and west from cur, checking  if they are in bounds of maze or are
// walls. If in bounds and is not a wall, the GridLocation is added to a Set of GridLocations
// that is returned.

Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;

    GridLocation loc;

    // north
    if (cur.row != 0) {
        loc = {cur.row-1, cur.col};
        if (maze.inBounds(loc) && maze[loc])    neighbors.add(loc);
    }

    // east
    if (cur.col != maze.numCols()) {
        loc = {cur.row, cur.col+1};
        if (maze.inBounds(loc) && maze[loc])    neighbors.add(loc);
    }

    // south
    if (cur.row != maze.numRows()) {
        loc = {cur.row+1, cur.col};
        if (maze.inBounds(loc) && maze[loc])    neighbors.add(loc);
    }

    // west
    if(cur.col != 0) {
        loc = {cur.row, cur.col-1};
        if (maze.inBounds(loc) && maze[loc])    neighbors.add(loc);
    }

    return neighbors;
}

// This function takes two parameters Grid maze and a Vector of GridLocations path. The function
// checks that the path, within the context of the maze, doesn't start at the entry, doesn't end
// at the exit, contains only valid moves, and doens't contain a loop. If any of these checks are
// failed, the function produces an error that prints which check is failed.

void validatePath(Grid<bool>& maze, Vector<GridLocation>& path) {
    if (path.isEmpty()) {
        error("Path is empty!");
    }

    // path doesn't start at entry
    if (path[0] != GridLocation {0, 0}) {
        error("Path doesn't start at entry");
    }


    // path doesn't end at exit
    if (path[path.size()-1] != GridLocation {maze.numRows()-1, maze.numCols()-1}) {
        error("Path doesn't end at exit");
    }

    // each location isn't a valid move from previous path
    for (int i = 0; i < path.size()-1; i++) {
        if (!generateValidMoves(maze, path[i]).contains(path[i+1]))
            error("Path is not a valid move from previous path");
    }

    // path contains a loop
    Set<GridLocation> pastLocs = {path[0]};
    for (int i = 1; i < path.size(); i++) {
        if (pastLocs.contains(path[i]))
            error("Path contains a loop");
        pastLocs.add(path[i]);
    }

}

// This function takes one parameter Grid maze and returns a Vector of GridLocations corresponding
// to a solution path, after solving the maze using a breadth-first search. This
// algorithm considers paths outtward from the entry location in a radial fashion until i finds the
// exit. Starting with one possible path starting at the entrance, the function iterates through
// possible moves from that path and queues new possible paths if the move is valid and doesn't create
// a loop. The function iterates through a growing queue until it reaches the exit, and returns
// the solution path.

Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    drawMaze(maze);

    Queue<Vector<GridLocation>> paths;
    Vector<GridLocation> path = {GridLocation {0, 0}};
    Set<GridLocation> pastMoves = {{0, 0}};

    paths.enqueue(path);

    while (!paths.isEmpty()) {
        path = paths.dequeue();
        GridLocation cur = path[path.size()-1];

        // return answer if ends at exit
        if (cur == GridLocation{maze.numRows()-1, maze.numCols()-1}) {
            highlightPath(path, "blue", 1);
            return path;
        }

        // generate possible moves from the last location in the path
        Set<GridLocation> moves = generateValidMoves(maze, path[path.size()-1]);

        // go through each possible move
        for (GridLocation move : moves) {
            // check that path doesn't revisit location
            if (!pastMoves.contains(move)) {
                pastMoves.add(move);
                Vector<GridLocation> newPath = path;
                newPath += move;
                paths.enqueue(newPath);
                highlightPath(newPath, "blue", 1);
            }
        }
    }

    highlightPath(path, "blue", 1);
    return path;
}


// This function takes one parameter Grid maze and returns a Vector of GridLocations corresponding
// to a solution path, after solving the maze using a depth-first search. This algorithm starts on
// a path continues along the path until it reaches an end, and moves on to other potential paths if
// unsuccessful. Starting with one possible path from the entrance, the function iterates through
// possible moves from that path and stacks new possible paths if the move is valid and doesn't create
// a loop. The function iterates through the stack until it reaches the exit, and returns
// the solution path.

Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    drawMaze(maze);

    Stack<Vector<GridLocation>> paths;
    Vector<GridLocation> path = {GridLocation {0, 0}};
    Set<GridLocation> pastMoves = {{0, 0}};

    paths.push(path);

    while (!paths.isEmpty()) {
        path = paths.pop();
        GridLocation cur = path[path.size()-1];

        // return answer if ends at exit
        if (cur == GridLocation{maze.numRows()-1, maze.numCols()-1}) {
            highlightPath(path, "blue", 1);
            return path;
        }

        // generate possible moves from the last location in the path
        Set<GridLocation> moves = generateValidMoves(maze, path[path.size()-1]);

        // go through each possible move
        for (GridLocation move : moves) {
            // check that path doesn't revisit location
            if (!pastMoves.contains(move)) {
                pastMoves.add(move);
                Vector<GridLocation> newPath = path;
                newPath += move;
                paths.push(newPath);
                highlightPath(newPath, "blue", 1);
            }
        }
    }

    highlightPath(path, "blue", 1);
    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines = stringSplit(readEntire(in), '\n');

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Vector<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution, hand-constructed maze") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution read from file, medium maze") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> notBeginAtEntry = { {1, 1}, {2, 1} };
    Vector<GridLocation> notEndAtExit = { {0, 0}, {1, 0}, {2, 0} };
    Vector<GridLocation> moveThroughWall = { {0 ,0}, {0, 1}, {1, 1}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, notBeginAtEntry));
    EXPECT_ERROR(validatePath(maze, notEndAtExit));
    EXPECT_ERROR(validatePath(maze, moveThroughWall));
}

PROVIDED_TEST("solveMazeBFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMazeDFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}


// student tests for generateValidMoves

STUDENT_TEST("generateValidMoves on location on the side of 3x3 grid with walls") {
    Grid<bool> maze = {{true, true, false},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

STUDENT_TEST("generateValidMoves on location on the corner of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{0, 1}, {1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("generateValidMoves on location on the corner of 3x3 grid with walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, false, true}};
    GridLocation corner = {2, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("generateValidMoves on location on the corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {false, true},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

// student tests for validatePath

STUDENT_TEST("validatePath on invalid solution, path doesn't start at entry") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("validatePath on invalid path, path doesn't end at exit") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> notEndAtExit = { {0, 0}, {1, 0}, {1, 2} };

    EXPECT_ERROR(validatePath(maze, notEndAtExit));
}

STUDENT_TEST("validatePath on invalid path, path contains non-valid moves: skip") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, false, true}};
    Vector<GridLocation> invalid = { {0, 0}, {1, 0}, {1, 1}, {2, 2} };

    EXPECT_ERROR(validatePath(maze, invalid));
}

STUDENT_TEST("validatePath on invalid path, path contains non-valid moves: wall") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, false, true}};
    Vector<GridLocation> wall = { {0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2} };

    EXPECT_ERROR(validatePath(maze, wall));
}

STUDENT_TEST("validatePath on invalid path, path contains non-valid moves: off-grid") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, false, true}};
    Vector<GridLocation> offGrid = { {0, 0}, {-1, 0}, {-1, 1}, {1, 0}, {1, 1}, {2, 1}, {2, 2} };

    EXPECT_ERROR(validatePath(maze, offGrid));
}

STUDENT_TEST("validatePath on invalid path, path contains a loop") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, false, true}};
    Vector<GridLocation> loop = { {0, 0}, {1, 0}, {1, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 2}};

    EXPECT_ERROR(validatePath(maze, loop));
}

// student tests for solveMazeBFS

STUDENT_TEST("solveMazeBFS on file 2x2") {
    Grid<bool> maze;
    readMazeFile("res/2x2.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeBFS on file 13x39") {
    Grid<bool> maze;
    readMazeFile("res/13x39.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// student tests for solvMazeDFS

STUDENT_TEST("solveMazeDFS on file 2x2") {
    Grid<bool> maze;
    readMazeFile("res/2x2.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on file 13x39") {
    Grid<bool> maze;
    readMazeFile("res/13x39.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}
