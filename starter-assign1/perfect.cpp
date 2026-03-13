/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* The smarterSum function takes one argument 'n' and calculates the
 * sum of proper divisors of 'n' excluding itself. To find divisors
 * a loop iterates over numbers from 2 to the square root of n, testing
 * for a zero remainder using the modulus operator %. If the number is
 * found to be a divisor, it is added to the sum along with the associated
 * pairwise factor. For values 1 and below, 0 is returned.
 */
long smarterSum(long n) {
    /* TODO: Fill in this function. */

    if (n <= 1)   return 0;

    long total = 1;
    for (long divisor = 2; divisor <= sqrt(n); divisor++) {
        if (n % divisor == 0) {
            total += divisor;

            long pairwise = n / divisor;
            if(pairwise != divisor) {
                total += pairwise;
            }
        }
    }
    return total;

}

/* The isPerfectSmarter function takes one argument 'n' and returns a
 * boolean (true/false) value indicating whether or not 'n' is perfect.
 * This is done similarly to isPerfect, except substituting the divisorSum
 * function for the smarterSum function for optimized calculations.
 */
bool isPerfectSmarter(long n) {
    /* TODO: Fill in this function. */

    return (n != 0) && (n == smarterSum(n));
}

/* The findPerfectsSmarter function takes one argument 'stop' and performs
 * an exhaustive search for perfect numbers over the range 1 to 'stop'.
 * This is done similarly to findPerfects, except substituting the isPerfect
 * function for the isPerfectSmarter function for optimized calculations.
 */
void findPerfectsSmarter(long stop) {
     /* TODO: Fill in this function. */

    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* The findNthPerfectEuclid function takes one argument n and returns
 * the nth perfect number, after performing an optimized algorithm to find
 * perfect numbers. This function iterates through numbers by powers of two
 * to check if each associated Mersenne number is prime, and then calculating
 * the corresponding perfect number.
 */
long findNthPerfectEuclid(long n) {
    long count = 0;

    for (long k = 1; ; k++) {
        long m = (pow(2, k) - 1);

        if (smarterSum(m) == 1) {
            count++;

            if (count == n)
                return (pow(2, k-1)*m);
        }
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}

STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {

    int smallest = 20000, largest = 160000;

    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfects(size));
    }

}

STUDENT_TEST("Test negative input -1 returns false") {
    EXPECT(!isPerfect(-1));
}

STUDENT_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(smarterSum(1), 0);
    EXPECT_EQUAL(smarterSum(6), 6);
    EXPECT_EQUAL(smarterSum(12), 16);
    EXPECT_EQUAL(smarterSum(25), 6);
}

STUDENT_TEST("Confirm expected numbers are perfect/non-perfect") {
    EXPECT(isPerfectSmarter(6));
    EXPECT(isPerfectSmarter(28));
    EXPECT(!isPerfectSmarter(12));
    EXPECT(!isPerfectSmarter(98765));
    EXPECT(!isPerfectSmarter(0));
    EXPECT(!isPerfectSmarter(1));
    EXPECT(isPerfectSmarter(33550336));
}

STUDENT_TEST("Multiple time trials of findPerfectsSmarter on increasing input sizes") {

    int smallest = 1875000, largest = 15000000;

    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfectsSmarter(size));
    }

}

STUDENT_TEST("Confirm expected numbers are perfect/non-perfect for Euclid method") {
    EXPECT_EQUAL(findNthPerfectEuclid(1), 6);
    EXPECT_EQUAL(findNthPerfectEuclid(2), 28);
    EXPECT_EQUAL(findNthPerfectEuclid(4), 8128);
    EXPECT(isPerfect(findNthPerfectEuclid(1)));
    EXPECT(isPerfect(findNthPerfectEuclid(2)));
    EXPECT(isPerfect(findNthPerfectEuclid(4)));
    EXPECT_EQUAL(findNthPerfectEuclid(0), 0);
    EXPECT_EQUAL(findNthPerfectEuclid(-1), 0);
    EXPECT(isPerfect(findNthPerfectEuclid(0)));
    EXPECT(isPerfect(findNthPerfectEuclid(-1)));
}
