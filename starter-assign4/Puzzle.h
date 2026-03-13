#pragma once

#include "Tile.h"
#include "direction.h"
#include "grid.h"
#include "map.h"

class Puzzle {
public:
    /**
     * @brief canMatchEdge checks if a specified tile matches with its neighbor in a specified direction.
     * @param tile The tile in question.
     * @param loc The location of the tile in question.
     * @param dir The direction of the neighbor with respect to tile
     *
     * @return true if neighbor tile in direction matches with tile, false if it does not
     */
    bool canMatchEdge(Tile tile, GridLocation loc, Direction dir) const;

    /**
     * @brief configure assigns the complement map of pairs and the grid,
     *        and clears the puzzle
     * @param numRows: the number of rows in the puzzle (usually 3)
     * @param numCols: the number of columns in the puzzle (usually 3)
     * @param pairs:
     */
    void configure(int numRows, int numCols, Map<std::string, std::string>& pairs);

    /**
     * @brief isFull: is the grid full?
     * @return true if all grid locations are filled, false otherwise
     */
    bool isFull() const;

    /**
     * @brief isEmpty: is the grid empty?
     * @return true if no grid locations are filled, false otherwise
     */
    bool isEmpty() const;

    /**
     * @brief canAdd: is it valid to add tile to the grid at the next unfilled location?
     *         valid means all edges of the tile match at that location
     * @param tile: a Tile
     * @return true if the tile can be added to produce a matched section of the grid
     */
    bool canAdd(Tile tile) const;

    /**
     * @brief add adds tile to the grid at the next unfilled location. Grid locations are
     *        filled left to right and then top to bottom. Validity of the match is not
     *        checked by add, call canAdd to confirm before add
     * @param tile: a Tile
     */
    void add(Tile tile);

    /**
     * @brief remove removes the last tile added (i.e. at the last filled location)
     * @return the tile that was removed is returned
     */
    Tile remove();

    /**
     * @brief tileAt returns the tile at the grid location loc
     * @param loc: a GridLocation to pull the tile from
     * @return
     */
    Tile tileAt(GridLocation loc) const;

    /**
     * @brief print prints out the puzzle in a human-readable form (useful for debugging)
     */
    void print() const;

private:
    /**
     * @brief isComplement returns true if the strings are complements of each other
     *        in the complement map. Ordering of one and two is irrelevant
     * @param one: one potential string for matching
     * @param two: the second potential match
     * @return true if they match, false otherwise
     */
    bool isComplement(std::string one, std::string two) const;

    /**
     * @brief matchesAt ensures that tile matches on all sides if it was placed at loc
     * @param tile: a Tile
     * @param loc: the location in the grid where the tile would be placed
     * @return: true if matched, false otherwise
     */
    bool canMatchAllEdges(Tile tile, GridLocation loc) const;

    /**
     * @brief locationForCount translates a count (0-8 for a 3x3 puzzle) into a grid location
     * @param count: 0 to (numRows * numCols) - 1
     * @return the grid location for the count
     */
    GridLocation locationForCount(int count) const;

    /**
     * @brief _grid is the underlying grid that holds all of the tiles in the puzzle
     */
    Grid<Tile> _grid;

    /**
     * @brief _complementMap is a map of matching tile edge strings. It is bidirectional,
     *        so that it maps A->a and a->A for each pair of strings
     */
    Map<std::string, std::string> _complementMap;

    /**
     * @brief _numFilled is the number of filled locations in the grid
     */
    int _numFilled;
};
