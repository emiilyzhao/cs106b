#pragma once

#include "direction.h" // for NORTH, EAST, SOUTH, WEST

#define NUM_SIDES 4

class Tile {
public:
    /* constructor Tile (overloaded)
     * The constructor is passed four strings that indicate the labels
     * for the tile's edges. These strings are aassigned to the
     * private member variables of the newly constructed Tile object.
     *
     * @param north The string label for the top edge of the tile
     * @param east The string label for the right edge of the tile
     * @param south The string label for the bottom edge of the tile
     * @param west The string label for the left edge of the tile
     */
    Tile(std::string north, std::string east, std::string south, std::string west);

    /* member function getEdge
     * Return the string label for the specified edge
     *
     * @param dir The desired edge: NORTH, EAST, SOUTH, or WEST
     *
     * @return The string label for the requested edge
     */
    std::string getEdge(Direction dir) const;

    /* member function rotate()
     * Updates the private member variables for tile edges to
     * simulate a quarter turn in the clockwise direction. The edge
     * label that was previously stored for west has moved to north,
     * what was south has moved to west, and so on.
     */
    void rotate();

    /* constructor Tile (default)
     * The default constructor. Constructs a Tile with empty edges (blank).
     */
    Tile();

    /* member function isBlank()
     * Checks if the tile is a blank placeholder.
     *
     * @return True if the tile is blank and false otherwise
     */
    bool isBlank() const;

    /* member function displayTileStr()
     * Returns a string that is a display representation of a tile,
     * with the correct orientation. E.g., tile "AbaB" rotated 3 times
     * would return a string that looks like this (with the newline after the "B"):
     *   b
     * A   a
     *   B
     *
     * @return A string in the above format
     */
    std::string displayTileStr() const;

    /* member function toString
     * Returns a string representation of the tile's edges in the form
     * "north-east-south-west".
     *
     * @return A string in the form of "north-east-south-west", based on the edges.
     */
    std::string toString() const;

    /* friend function operator<<
     * Overloads the "<<" operator to print out the string
     * representation of the tile (e.g. toString())
     *
     * @param out The output stream to send the string to
     * @param tile The tile to output
     *
     * @return The function returns out
     */
    friend bool operator< (const Tile& lhs, const Tile& rhs) { return lhs._id < rhs._id; }
    friend bool operator== (const Tile& lhs, const Tile& rhs) { return lhs._id == rhs._id; }
    friend std::ostream &operator<<(std::ostream &out, const Tile &tile) { return out << tile.toString(); }

private:
    /* member variables _north, _east, _south, and _west
     * each is the string label for the corresponding edge
     */
    std::string _north;
    std::string _east;
    std::string _south;
    std::string _west;

    std::string _id; // unique identifier required for comparison (Set/Map)
};
