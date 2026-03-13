/*
 * This file contains assignment "Merge," containing functions
 * binaryMerge, naiveMultiMerge, and recMultiMerge, along with
 * associated provided and student tests.
 */
#include <iostream>    // for cout, endl
#include "queue.h"
#include "SimpleTest.h"
using namespace std;

/*
 * This function takes two queues of integers, a and b, and
 * returns a queue of integers of all the integers in a and b
 * sorted by size, while verifying that a and b were already sorted.
 * This is done by iterating through a and b, comparing the first terms,
 * and dequeuing the smaller term to queue onto the result. The function
 * also stores the last term dequeued for either queue, and calls an
 * error if the following term is smaller, indicating an out of order queue.
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    int holder;
    int lastA = INT_MIN;
    int lastB = INT_MIN;

    // compare values to add to result, call error if not ordered
    while (!a.isEmpty() && !b.isEmpty()) {
        if (a.peek() <= b.peek()) {
            holder = a.dequeue();
            if (holder < lastA)
                error("Queue A not sorted");
            lastA = holder;
        }

        else {
            holder = b.dequeue();
            if (holder < lastB)
                error("Queue B not sorted");
            lastB = holder;
        }

        result.enqueue(holder);
    }

    // a is empty
    while (!a.isEmpty()) {
        holder = a.dequeue();
        if (holder < lastA)
            error("Queue A not sorted");
        lastA = holder;
        result.enqueue(holder);

    }

    // b is empty
    while (!b.isEmpty()) {
        holder = b.dequeue();
        if (holder < lastB)
            error("Queue B not sorted");
        lastB = holder;
        result.enqueue(holder);
    }

    return result;
}

/*
 * The function naiveMultiMerge assumes you have a correctly working
 * binaryMerge function, which this function calls to iteratively
 * merge a collection of sequences, starting off with an empty sequence
 * and repeatedly merging in an additional sequence until all are merged.
 * The code below is provided to you is implemented correctly and you
 * should not need to modify it.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/*
 * This function takes a vector of queues of integers,
 * and merges all integers in all queues to return a queue
 * of integers in order. This is done by splitting the vector
 * in half recursively until 2 queues are left, and then calling
 * binaryMerge to merge them. This is repeated with larger and
 * larger queues until all the queues in the vector are sorted
 * and returned.
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {

    int k = all.size();
    Vector<Queue<int>> firstHalf = all.subList(0, k/2);
    Vector<Queue<int>> secondHalf = all.subList(k/2, k - k/2);

    // base case, both vectors are empty
    if (k == 0)
        return {};

    // base case, only one queue left
    if (k == 1)
        return all[0];

    return binaryMerge(recMultiMerge(firstHalf), recMultiMerge(secondHalf));

}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}


// student tests for binaryMerge
STUDENT_TEST("binaryMerge, either queue is empty") {
    Queue<int> a = {};
    Queue<int> b = {1, 2, 3};
    Queue<int> expected = {1, 2, 3};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
    EXPECT_EQUAL(binaryMerge(a, a), {});

}

STUDENT_TEST("binaryMerge, either queue is out of order, expect error") {
    Queue<int> a = {1, 2, 3};
    Queue<int> b = {1, 3, 2};
    EXPECT_ERROR(binaryMerge(a, b));
    EXPECT_ERROR(binaryMerge(b, a));
    EXPECT_ERROR(binaryMerge(b, b));
}

STUDENT_TEST("binaryMerge, one queue 'within' the other") {
    Queue<int> a = {2, 2, 2};
    Queue<int> b = {1, 2, 3};
    Queue<int> expected = {1, 2, 2, 2, 2, 3};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, one queue 'outside' the other") {
    Queue<int> a = {1, 2, 3};
    Queue<int> b = {4, 5, 6};
    Queue<int> expected = {1, 2, 3, 4, 5, 6};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, identical queues") {
    Queue<int> a = {1, 2, 3};
    Queue<int> expected = {1, 1, 2, 2, 3, 3};
    EXPECT_EQUAL(binaryMerge(a, a), expected);
}

STUDENT_TEST("binaryMerge, queues overlap") {
    Queue<int> a = {1, 2, 3};
    Queue<int> b = {2, 3, 4};
    Queue<int> expected = {1, 2, 2, 3, 3, 4};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, find big O with different sized queues") {
    Queue<int> a;
    for (int i = 5000000; i < 100000000; i*=2) {
        a.clear();
        for (int b = 0; b < i; b++)
            a.enqueue(b);
        TIME_OPERATION(a.size(), binaryMerge(a, {}));
    }
}

// student tests for nativeMultiMerge

STUDENT_TEST("naiveMultiMerge: predict empty vector returned for 0 queues") {
    Vector<Queue<int>> queues;
    EXPECT_EQUAL(naiveMultiMerge(queues), {});
}

STUDENT_TEST("naiveMultiMerge: predict empty vector returned for empty queues") {
    Vector<Queue<int>> queues = {{}, {}, {}};
    EXPECT_EQUAL(naiveMultiMerge(queues), {});
}

STUDENT_TEST("naiveMultiMerge, find big O with different sized queues (vary n, constant k = 1)") {
    Vector<Queue<int>> a = {};

    for (int n = 5000000; n < 100000000; n*=2) {
        a.clear();
        a.add({});
        for (int i = 0; i < n; i++)
            a[0].enqueue(i);
        TIME_OPERATION(a[0].size(), naiveMultiMerge(a));
    }
}

STUDENT_TEST("naiveMultiMerge, find big O with different number of queues (vary k, constant n = 1000)") {
    int n = 1000;
    Vector<Queue<int>> a = {};

    for (int k = 1; k < n; k*=2) {
        a.clear();

        for (int i = 0; i < k; i++) {
            Queue<int> b = {};

            for (int j = 0; j < n; j++) {
                b.enqueue(j);
            }

            a.add(b);
        }

        TIME_OPERATION(k, naiveMultiMerge(a));
    }
}

// student tests for recMultiMerge

STUDENT_TEST("recMultiMerge, matches results from naiveMultiMerge") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

STUDENT_TEST("recMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
        {1, 5, 9, 9, 12},
        {5},
        {},
        {-5, -5},
        {3402}
    };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(recMultiMerge(all), expected);
}

STUDENT_TEST("recMultiMerge, matches naiveMultiMerge with small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
        {1, 5, 9, 9, 12},
        {5},
        {},
        {-5, -5},
        {3402}
    };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

STUDENT_TEST("recMultiMerge, confirm operates in O(N logK) time with different sized queues (vary n, constant k = 1)") {
    Vector<Queue<int>> a = {};

    for (int n = 5000000; n < 100000000; n*=2) {
        a.clear();
        a.add({});
        for (int i = 0; i < n; i++)
            a[0].enqueue(i);
        TIME_OPERATION(a[0].size(), recMultiMerge(a));
    }
}

STUDENT_TEST("recMultiMerge, confirm operates in O(N logK) time with different number of queues (vary k, constant n = 1000)") {
    int n = 1000;
    Vector<Queue<int>> a = {};

    for (int k = 1; k < n; k*=2) {
        a.clear();

        for (int i = 0; i < k; i++) {
            Queue<int> b = {};

            for (int j = 0; j < n; j++) {
                b.enqueue(j);
            }

            a.add(b);
        }

        TIME_OPERATION(k, recMultiMerge(a));
    }
}
