/*
 * This file contains assignment "Sierpinski," containing functions
 * fillBlackTriangle and drawSierpinskiTriangle, along with the
 * associated provided test case that calls an Interactive Demo to test.
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

/*
 * This function takes 5 parameters: a window, 3 points, and the order of
 * Sierpinski triangles. The function returns an integer of the number of
 * Sierpiinski triangles are drawn for that order, while drawing them in the
 * window. This is done by calculating the points of new triangles, while
 * recursievly calling itself on the new triangles with one less order. A base
 * case of order=0 is implemneted, which returns 1 as no more triangles are drawn.
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order == 0) {
        fillBlackTriangle(window, one, two, three);
        return 1;
    }

    GPoint midPoint1;
    GPoint midPoint2;
    int total = 0;

    // triangle 1, one constant
    midPoint1 = {(one.x + two.x)/2, (one.y + two.y)/2};
    midPoint2 = {(one.x + three.x)/2, (one.y + three.y)/2};
    total += drawSierpinskiTriangle(window, one, midPoint1, midPoint2, order-1);

    // triangle 2, two constant
    midPoint2 = {(two.x + three.x)/2, (two.y + three.y)/2};
    total += drawSierpinskiTriangle(window, midPoint1, two, midPoint2, order-1);

    // triangle 1, three constant
    midPoint1 = {(one.x + three.x)/2, (one.y + three.y)/2};
    total += drawSierpinskiTriangle(window, midPoint1, midPoint2, three, order-1);

    return total;

}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all your testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

