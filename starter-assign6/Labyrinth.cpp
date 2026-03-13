#include "Labyrinth.h"
using namespace std;

/* this helper function calculates the next move for the location,
 * given the move stated
 */
MazeCell* nextMove(MazeCell* loc, char move) {
    if (move == 'N')
        return loc->north;
    if (move == 'E')
        return loc->east;
    if (move == 'S')
        return loc->south;
    if (move == 'W')
        return loc-> west;

    return nullptr;
}

/* this helper function changes the array of booleans according to
 * whether an object is found, and which type of object is found if so.
 */
void findItem (MazeCell* loc, bool found[3]) {
    if (loc->whatsHere == Item::SPELLBOOK)
        found[0] = true;
    if (loc->whatsHere == Item::POTION)
        found[1] = true;
    if (loc->whatsHere == Item::WAND)
        found[2] = true;
}

/* this function outputs whether the path is valid or not. this is done
 * by iterating through each move in the string and determining whether they
 * are valid. At the end, if all objects are collected, the path is valid.
 */
bool isPathToFreedom(MazeCell* start, const string& moves) {

    bool found[3] = {false, false, false};

    MazeCell* loc = start;
    findItem(loc, found);

    // iterate through all moves
    for (int i = 0; i < moves.size(); i++) {

        loc = nextMove(loc, moves[i]);

        // if its not valid
        if (loc == nullptr) {
            return false;
        }

        findItem(loc, found);
    }

    for (bool objects : found)
        if (objects != true)
            return false;

    return true;
}

/* * * * * * Test Cases Below This Point * * * * * */
#include "GUI/SimpleTest.h"
#include "Demos/MazeGenerator.h"

/* * * * * Provided Tests Below This Point * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we write 'delete maze;'
     * rather than what's shown above?
     */
}

PROVIDED_TEST("Checks paths in the sample maze.") {
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);
}

PROVIDED_TEST("Can't walk through walls.") {
    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);
}

PROVIDED_TEST("Works when starting on an item.") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);
}

/* Printer for items. */
ostream& operator<< (ostream& out, Item item) {
    if (item == Item::NOTHING) {
        return out << "Item::NOTHING";
    } else if (item == Item::WAND) {
        return out << "Item::WAND";
    } else if (item == Item::POTION) {
        return out << "Item::POTION";
    } else if (item == Item::SPELLBOOK) {
        return out << "Item::SPELLBOOK";
    } else {
        return out << "<unknown item type>";
    }
}
