/**
 *  This file contains the contexts of the jpeg file decoder.
 *  The only function, loadJpeg, processes a GCanvas by reference
 *  and a string of the filename to create a grid of integers
 *  that represents the image.
 */

#include<iostream>
#include "grid.h"
#include "gcanvas.h"
#include "huffman.h"

using namespace std;

void loadJpeg(GCanvas& img, string filename) {
    // Your Code Here
    // GImage gim(filename);
    // Grid<int>grid(gim.getHeight(), gim.getWidth());
    // for (int r = 0; r < grid.numRows(); r++) {
    //     for (int c = 0; c < grid.numCols(); c++) {
    //         grid[r][c] = gim.getPixel(c, r);
    //     }
    // }
    Grid<int> grid;
    img.fromGrid(grid);
}
