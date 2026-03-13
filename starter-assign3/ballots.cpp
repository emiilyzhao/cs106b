/*
 * This file corresponds with assignment "Ballot counting," and contains functions
 * countAllOrderings, countGoodOrderings, countGoodOrderings (helper), and all
 * associated provided and student test cases.
 */
#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

/*
 * This function take two parameters, int a and int b, which
 * correspond to the number of votes for candidate a and candidate b,
 * and returns an integer of the number of possible orderings for
 * tallying an election with these ballot counts. The function
 * recursively calls itself with the next sequence, whether a/b is next.
 * If either vote count is empty, there only remains one sequence.
 */
int countAllOrderings(int a, int b) {
    if (a == 0 || b == 0) {
        return 1;
    }

    return (countAllOrderings(a-1, b) + countAllOrderings(a, b-1));

}

/*
 * This function is the helper function for countGoodOrderings.
 * It takes 3 parameters, int a and int b, which
 * correspond to the number of votes for candidate a and candidate b,
 * and returns an integer of the number of good orderings. A good
 * ordering is defined as one where candidate A is in the lead throughout.
 * This is done by recursively calling itself that calculates
 * additional possible orderings, while keeping in account how much a is in
 * the lead for. if a is not in the lead, 0 is returned. Otherwise, if all orderings
 * are accounted for and a is in the lead, 1 is returned.
 */
int countGoodOrderings(int a, int b, int lead) {

    // track how many iterations a is in lead for
    int total = 0;

    // a is not in lead so far
    if (lead <= 0)
        return 0;

    // over, finished iteration
    if (a == 0 && b == 0)
        return 1;

    // still more to iterate through, start with a
    if (a > 0)
        total += countGoodOrderings(a-1, b, lead+1);
    if (b > 0)
        total += countGoodOrderings(a, b-1, lead-1);

    return total;
}

/*
 * This function takes two parameters, int a and int b, which correspond
 * to the number of votes for candidate a and candidate b, and returns the
 * number of good orderings. If a is not in the lead, 0 is returned because
 * this is not a good ordering. Otherwise, it recursively calls the helper function
 * that defines whether the function is good or not, considering the next 2 possible
 * orderings where a or b are next.
 */
int countGoodOrderings(int a, int b) {

    // a is not in lead overall
    if (a <= b)
        return 0;

    // must start with a
    return countGoodOrderings(a-1, b, 1);
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("countAllOrderings, two A one B") {
   EXPECT_EQUAL(countAllOrderings(2, 1), 3);
}

PROVIDED_TEST("countGoodOrderings, two A one B") {
   EXPECT_EQUAL(countGoodOrderings(2, 1), 1);
}

// student tests for countAllOrderings

STUDENT_TEST("countAllOrderings, one A two B") {
    EXPECT_EQUAL(countAllOrderings(1, 2), 3);
}

STUDENT_TEST("countAllOrderings, two A two B") {
    EXPECT_EQUAL(countAllOrderings(2, 2), 6);
}

STUDENT_TEST("countAllOrderings, one A three B") {
    EXPECT_EQUAL(countAllOrderings(1, 3), 4);
}

STUDENT_TEST("countAllOrderings, three A one B") {
    EXPECT_EQUAL(countAllOrderings(3, 1), 4);
}

// student tests for countGoodOrderings

STUDENT_TEST("countGoodOrderings, one A two B") {
    EXPECT_EQUAL(countGoodOrderings(1, 2), 0);
}

STUDENT_TEST("countGoodOrderings, two A two B") {
    EXPECT_EQUAL(countGoodOrderings(2, 2), 0);
}

STUDENT_TEST("countGoodOrderings, one A three B") {
    EXPECT_EQUAL(countGoodOrderings(1, 3), 0);
}

STUDENT_TEST("countGoodOrderings, three A one B") {
    EXPECT_EQUAL(countGoodOrderings(3, 1), 2);
}

// student test for Bertrand's theorem

STUDENT_TEST("Verify Bertrand's theorem with countGoodOrderings & countAllOrderings") {
    for (int a = 1; a < 15; a++) {
        for (int b = 0; b < a; b++) {
            EXPECT_EQUAL((double(countGoodOrderings(a, b))/countAllOrderings(a, b)), double(a-b)/(a+b));
        }
    }
}

