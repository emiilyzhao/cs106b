/*
 * This file contains the "Balanced Operators," assignment, containing
 * the operatorsFrom, operatorsAreMatched, and isBalanced functions, along
 * with corresponding provided and student tests.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "SimpleTest.h"

using namespace std;

/*
 * This function takes one string parameter and returns a string
 * containing only the (), [], and {} operators. This is done by recursively
 * analyzing the first character and calling itself on the rest of the string,
 * only returning the first character if it is found to be an operator. A base
 * case of if the string is empty is defined, which returns an empty string.
 */
string operatorsFrom(string str) {
    if (str.length()==0)
        return "";
    char first = str[0];
    if (first == '[' || first == ']' || first == '{' || first == '}' || first == '(' || first == ')')
        return charToString(first) + operatorsFrom(str.substr(1, str.length()));
    else
        return operatorsFrom(str.substr(1, str.length()));
}

/*
 * This function takes one string parameter and returns a boolean of
 * whether or not all operators are matched and valid. The function
 * recursively matches the first opening operator with the cooresponding
 * closing operator, and then calls itself on the substring inside and outside
 * the match, while considering the depth of the match. A base case of if the
 * string is empty is implemented, in which true is returned as all operators are
 * successfully matched.
 */
bool operatorsAreMatched(string ops) {
    if (ops.length()==0)
        return true;

    // match up operators
    char open = ops[0];
    char close;
    if (open == '(')
        close = ')';
    else if (open == '[')
        close = ']';
    else if (open == '{')
        close = '}';
    else
        return false;

    int match = -1;
    int interior = 0;
    for (int i = 0; i < ops.length(); i++) {
        if (ops[i] == open)
            interior++;
        if (ops[i] == close)
            interior--;

        if (interior == 0) {
            match = i;
            break;
        }

    }

    if (match <= 0)
        return false;

    return operatorsAreMatched(ops.substr(1, match-1)) &&
           operatorsAreMatched(ops.substr(match+1));
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

// student tests for operatorsFrom

STUDENT_TEST("operatorsFrom on an example w/ all three operators") {
    EXPECT_EQUAL(operatorsFrom("vec{([3]3"), "{([]");
}

STUDENT_TEST("operatorsFrom on an example w/ all open/close operators") {
    EXPECT_EQUAL(operatorsFrom("vec{([3]})3"), "{([]})");
}

STUDENT_TEST("operatorsFrom on an example w/ all operators at beginning/end") {
    EXPECT_EQUAL(operatorsFrom("[vec[3]"), "[[]");
}

// student tests for operatorsAreMatched

STUDENT_TEST("operatorsAreMatched on matched examples without nested operators") {
    EXPECT(operatorsAreMatched("()[]{}"));
    EXPECT(operatorsAreMatched("{}()[]"));
    EXPECT(operatorsAreMatched("[]{}()"));
}

STUDENT_TEST("operatorsAreMatched on matched examples containing nested operators") {
    EXPECT(operatorsAreMatched("([{}])"));
    EXPECT(operatorsAreMatched("([([]){}])"));
    EXPECT(operatorsAreMatched("([{}][])"));
}

STUDENT_TEST("operatorsAreMatched on non-matched nested example") {
    EXPECT(!operatorsAreMatched("([{])"));
    EXPECT(!operatorsAreMatched("([(]){}])"));
    EXPECT(!operatorsAreMatched("([}][])"));
}

STUDENT_TEST("operatorsAreMatched on non-matched without nested example") {
    EXPECT(!operatorsAreMatched("((]"));
    EXPECT(!operatorsAreMatched("()}"));
    EXPECT(!operatorsAreMatched("([]"));
}
