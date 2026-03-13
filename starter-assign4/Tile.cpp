/*
 *  This file contains the implementation of the Tile class.
 */

#include "Tile.h"
#include "SimpleTest.h"

using namespace std;

Tile::Tile(string n, string e, string s, string w) {
    _id = n + e + s + w;    // sets an id for internal use, do not change this line!

    _north = n;
    _east = e;
    _south = s;
    _west = w;
}

string Tile::getEdge(Direction dir) const {
    if (dir == NORTH)   return _north;
    if (dir == EAST)    return _east;
    if (dir == SOUTH)    return _south;
    return _west;
}

void Tile::rotate() {
    string holder = _north;
    _north = _west;
    _west = _south;
    _south = _east;
    _east = holder;
}


/* The functions below have already been written for you! */

Tile::Tile() : Tile("","","","") {}    // default constructor creates blank tile

bool Tile::isBlank() const {
    return (_north == "" && _east == "" && _south == "" && _west == "");
}

string Tile::displayTileStr() const{
    string s;
    s += "  " + _north + "\n";
    s += _west;
    s += "   " + _east + "\n";
    s += "  " + _south + "\n";
    return s;
}

string Tile::toString() const {
    return _north + "-" + _east + "-" + _south + "-" + _west;
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Testing Tile constructor") {
    Tile t1("red", "green", "blue", "yellow");
    EXPECT_EQUAL(t1.toString(), "red-green-blue-yellow");
}

PROVIDED_TEST("Testing Tile::getEdge()") {
    Tile t1("A", "B", "C", "D");
    EXPECT_EQUAL(t1.getEdge(NORTH), "A");
    EXPECT_EQUAL(t1.getEdge(EAST), "B");
    EXPECT_EQUAL(t1.getEdge(SOUTH), "C");
    EXPECT_EQUAL(t1.getEdge(WEST), "D");
}

PROVIDED_TEST("Testing Tile::rotate()") {
    // Test single rotate
    Tile t1("A", "B", "C", "D");
    t1.rotate();
    EXPECT_EQUAL(t1.getEdge(NORTH), "D");
    EXPECT_EQUAL(t1.getEdge(EAST), "A");
    EXPECT_EQUAL(t1.toString(), "D-A-B-C");
    t1.rotate();
    EXPECT_EQUAL(t1.toString(), "C-D-A-B");

    // Test rotate full circle
    Tile t2("E", "F", "G", "H");
    string orig = t2.toString();
    for (int i = 0; i < NUM_SIDES; i++)
        t2.rotate();
    EXPECT_EQUAL(t2.toString(), orig);

    // Test rotations greater than a full circle
    Tile t3("1", "2", "3", "4");
    for (int i = 0; i < 6; i++)
        t3.rotate();
    EXPECT_EQUAL(t3.toString(), "3-4-1-2");
}

PROVIDED_TEST("Testing blank Tile") {
    Tile empty;
    EXPECT(empty.isBlank());
    EXPECT_EQUAL(empty.getEdge(NORTH), "");
    EXPECT_EQUAL(empty.toString(), "---");
    empty.rotate();
    EXPECT_EQUAL(empty.toString(), "---");
}

// student tests

STUDENT_TEST("getEdge works after multiple rotations") {
    Tile tile("N", "E", "S", "W");

    tile.rotate();
    EXPECT_EQUAL(tile.getEdge(NORTH), "W");
    EXPECT_EQUAL(tile.getEdge(EAST),  "N");
    EXPECT_EQUAL(tile.getEdge(SOUTH), "E");
    EXPECT_EQUAL(tile.getEdge(WEST),  "S");

    tile.rotate();
    EXPECT_EQUAL(tile.getEdge(NORTH), "S");
    EXPECT_EQUAL(tile.getEdge(EAST),  "W");
    EXPECT_EQUAL(tile.getEdge(SOUTH), "N");
    EXPECT_EQUAL(tile.getEdge(WEST),  "E");
}

STUDENT_TEST("rotating 4 times returns to the original tile") {
    Tile tile("N", "E", "S", "W");
    Tile copy = tile;

    tile.rotate();
    tile.rotate();
    tile.rotate();
    tile.rotate();

    EXPECT_EQUAL(tile, copy);
}

STUDENT_TEST("rotating preserves string conversion") {
    Tile tile("N", "E", "S", "W");

    tile.rotate();
    EXPECT_EQUAL(tile.toString(), "W-N-E-S");
}
