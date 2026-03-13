#pragma once

/* Needed for voting.cpp */
#include "vector.h"
#include "Puzzle.h"
#include "set.h"

struct District {
    std::string name;
    int numBlueVotes;
    int numRedVotes;

    District(std::string name, int numBlueVotes, int numRedVotes) {
        this->name = name;
        this->numBlueVotes = numBlueVotes;
        this->numRedVotes = numRedVotes;
    }
};

struct Precinct {
    int numBlue;
    int numRed;

    Precinct(double numBlue, double numRed) {
        this->numBlue = numBlue;
        this->numRed = numRed;
    }
};

bool isMapGerrymandered(Vector<District>& districts);
bool isMapSusceptible(Vector<Precinct>& precincts);
