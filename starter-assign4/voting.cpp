// This file contains the assignment Voting Power.
// This assignment explores the voting block system with the Banzhaf power index,
// which expresses a block's voting power by the amount of critical votes they
// have. A vote is considered critical if their joining in a coalition is the
// deciding factor of whether that coalition wins the majority of votes.

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "voting.h"
#include "SimpleTest.h"
using namespace std;

// This function is a helper function for function computePowerIndexes.
// This function takes 5 parameters, a vector of blocks passed by reference, the targeted
// block that is being analyzed, the index of the blocks that analyzed as
// potentially joining the coalition, the sum of votes in the coalition, and
// the votes needed to make a majority.
// The function returns an integer: 0 if the block is not critical in the coalition
// analyzed, and 1 if the block is critical. 2 base cases are passed: when the majority
// is already won, and if there are no more blocks to iterate through. The function then
// recursively iterates through the remaining combinations with blocks in the vector,
// avoiding the target itself.
int combination (Vector<int>& blocks, int target, int i, int sum, int majority) {

    // base case: makes majority
    if (sum >= majority)
        return 0;

    // base case: made it to end, no more to iterate through
    if (i == blocks.size()) {
        if (sum < majority && (sum + blocks[target] >= majority))
            return 1;
        return 0;
    }


    if (i == target)
        return combination(blocks, target, i+1, sum, majority);

    return combination(blocks, target, i+1, sum+blocks[i], majority) + combination(blocks, target, i+1, sum, majority);
}

// This function takes one parameter, a vector of integers representing the number of votes
// in a block, and returns a vector containing the Banshaf power indexes for each block.
// This is done through recursion, where a helper function is called to find the amount of
// critical votes the block has for all combinations possible. This function then totals
// the number of critical votes for each block and determines the ratio that is the Banzhaf
// power index
Vector<int> computePowerIndexes(Vector<int>& blocks) {

    Vector <int> result(blocks.size());
    Vector <int> critical(blocks.size());

    int total = 0;

    // find total & majority
    for (int block : blocks)
        total += block;
    int majority = total/2 + 1;

    // iterate through each block
    for (int i = 0; i < blocks.size(); i++) {
        critical[i] = combination(blocks, i, 0, 0, majority);
    }

    // determine total number of critical
    int totalCritical = 0;
    for (int count : critical)
        totalCritical += count;

    // determine ratio for each block
    for (int i = 0; i < result.size(); i++) {
        if (totalCritical == 0)
            result[i] = 0;
        else {
            result[i] = (critical[i]*100 / totalCritical);
        }
    }


    return result;
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test power index, blocks 50-49-1") {
    Vector<int> blocks = {50, 49, 1};
    Vector<int> expected = {60, 20, 20};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks Hempshead 1-1-3-7-9-9") {
    Vector<int> blocks = {1, 1, 3, 7, 9, 9};
    Vector<int> expected = {0, 0, 0, 33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-NY 55-38-39") {
    Vector<int> blocks = {55, 38, 29};
    Vector<int> expected = {33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-GA 55-38-16") {
    Vector<int> blocks = {55, 38, 16};
    Vector<int> expected = {100, 0, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Time power index operation") {
    Vector<int> blocks;
    for (int i = 0; i < 15; i++) {
        blocks.add(randomInteger(1, 10));
    }
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}

// student test: time operation

STUDENT_TEST("Time operation test to find big O of computePowerIndex") {

    Vector <int> sizes = {1, 5, 10, 15, 20, 25, 26, 27, 28, 29, 30};

    for (int i = 1; i < sizes.size(); i ++) {
        Vector<int>blocks;
        for (int n = 1; n < sizes[i]; n++) {
            blocks.add(randomInteger(1, 10));
        }
        TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
    }
}
