/* This file corresponds with "Search Engine" in assignment 2, containing functions cleanToken,
 * gatherToken, buildIndex, findQueryMatches, and searchEngine, along with corresponding provided
 * and student test cases.
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// This function takes one parameter string s and returns a string of a clean token version of s.
// The clean version contains only lowercase letters and digits of all the characters in s.
// This is implemented by iterating through every character in s and appending it to a result string
// only if it is a letter, converted to lowercase form, or a digit. The result is returned.

string cleanToken(string s) {
    string result;

    for (char c : s) {
        if (isalpha(c) || isdigit(c)) {
            result += tolower(c);
        }
    }
    return result;
}

// This function takes one parameter string text and returns a set of strings corresponding to unique
// tokens from text. First, text is tokenized and split into words using the spaces as delimiters.
// Next, the tokens are cleaned with function cleanToken and stored into a set if they are not
// empty strings. The resulting set is returned.

Set<string> gatherTokens(string text) {
    Set<string> tokens;

    Vector<string> split = stringSplit(text, " ");

    // clean and then add to set if not empty
    for (string word : split) {
        word = cleanToken(word);
        if (word.length() > 0)
            tokens.add(word);
    }

    return tokens;
}

// This function takes two parameters, string dbfile and A Map of strings mapping onto Sets of strings
// and returns an integer of the number of documents processed. First, it reads the content from
// the database file and processes it into the form of an inverted index. Next, it iterates through
// every other line in the file, starting from the second. For each word in this line, it adds the
// corresponding webpage, located in the line before, to map onto the word. After every other line,
// the number of documents that have been processed is incremented, and after iterating through the
// entire file, the total number of documents is returned.

int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    int docs = 0;

    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines = stringSplit(readEntire(in), '\n');

    for (int i = 1; i < lines.size(); i += 2) {

        Set<string> words = gatherTokens(lines[i]);

        for (const string& word : words) {
            index[word].add(lines[i-1]);
        }

        docs++;
    }
    return docs;
}

// The function takes two parameters an index as a Map of strings mapping onto sets of strings, and a
// query string. First, the query is split into individual queries with spaces as delimiters. A Set of
// strings is created as a result to be returned. Next, we iterate through each query, creating a set
// of matches for each one cleaned. If the query begins with "+", the result contains matches that
// intersect between all queries, and if the query begins with "-", the result contains matches with
// that term removed from the result. Otherwise, the result contains matches unionized between all queries.

Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;
    Vector<string> queries = stringSplit(query, " ");


    for (const string& query : queries) {

        Set<string> matches = index[cleanToken(query)];

        // results much match both terms
        if (query[0] == '+') {
            result.intersect(matches);
        }

        // results must match one term without matching the other
        else if (query[0] == '-') {
            result.difference(matches);
        }

        // regular
        else {
            result.unionWith(matches);
        }

    }

    return result;
}

// This function contains a search engine, implementing a console program that constructs an inverted
// index from the database file parameter, prints how many web pages were processed, and enters a loop
// to prompt the user to enter a query, finding matching pages to each query and printing the URL.
// to terminate the program, the user must enter an empty string "".

void searchEngine(string dbfile) {

    cout<<"Stand by while building index..."<<endl;
    Map<string, Set<string>> index;
    cout<<"Indexed " + integerToString(buildIndex(dbfile, index)) + " pages containing " + integerToString(index.size()) + " unique terms.";

    while (true) {
        string query = getLine("\n\nEnter query sentence (RETURN/ENTER to quit): ");

        if (query == "")
            break;

        Set<string> result = findQueryMatches(index, query);

        cout<<"Found " + integerToString(result.size()) + " matching pages"<<endl;
        cout<<result<<endl;
    }

}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


// student tests for cleanToken

STUDENT_TEST("cleanToken on strings of letters and digits with other characters in beginning, middle, end") {
    EXPECT_EQUAL(cleanToken("hello."), "hello");
    EXPECT_EQUAL(cleanToken(".WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS1.06B"), "cs106b");
}

STUDENT_TEST("cleanToken on strings of capital letters in beginning, middle, and end") {
    EXPECT_EQUAL(cleanToken("Hello"), "hello");
    EXPECT_EQUAL(cleanToken("woRld"), "world");
    EXPECT_EQUAL(cleanToken("worlD"), "world");
    EXPECT_EQUAL(cleanToken("CS1.06B"), "cs106b");
}

STUDENT_TEST("cleanToken on strings of all capital with punctuation, all letters must be modified") {
    EXPECT_EQUAL(cleanToken(".HELLO"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD."), "world");
    EXPECT_EQUAL(cleanToken("CS1.06B"), "cs106b");
}

// student tests for gatherTokens

STUDENT_TEST("gatherTokens with duplicated strings") {
    Set<string> expected = {"d", "go", "gophers"};
    EXPECT_EQUAL(gatherTokens("d go d gophers"), expected);
}

STUDENT_TEST("gatherTokens cleans tokens, removes special characters and makes lowercase") {
    Set<string> expected = {"hello", "there", "im", "18", "years", "0ld"};
    EXPECT_EQUAL(gatherTokens("HELLO there I'm 18 years 0ld!"), expected);
}

STUDENT_TEST("gatherTokens with duplicates without punctuation and upper cases, only one token remains") {
    Set<string> tokens = gatherTokens("Hello HELLO !HellO! hello!");
    Set<string> expected = {"hello"};

    EXPECT_EQUAL(tokens, expected);
    EXPECT_EQUAL(tokens.size(), 1);
}

// student tests for buildIndex

STUDENT_TEST("TIME_OPERATION test on buildIndex from website.txt") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    TIME_OPERATION(nPages, buildIndex("res/tiny.txt", index));
}

STUDENT_TEST("buildIndex from website.txt, 36 pages") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 36);
}

STUDENT_TEST("buildIndex from website.txt, contains first and last keys, and last key from longest line") {
    Map<string, Set<string>> index;
    buildIndex("res/website.txt", index);
    EXPECT(index.containsKey("we"));
    EXPECT(index.containsKey("insight"));
    EXPECT(index.containsKey("without"));
}

// student tests for findQueryMatches

STUDENT_TEST("findQueryMatches from tiny.txt, single word query of first and last words from first and last sites") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> matchesMilk = findQueryMatches(index, "milk");
    EXPECT_EQUAL(matchesMilk.size(), 1);

    Set<string> matchesFish = findQueryMatches(index, "fish");
    EXPECT_EQUAL(matchesFish.size(), 3);
    EXPECT(matchesFish.contains("www.dr.seuss.net"));
    EXPECT(matchesFish.contains("www.shoppinglist.com"));
    EXPECT(matchesFish.contains("www.bigbadwolf.com"));
}

STUDENT_TEST("findQueryMatches from tiny.txt, words not found") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> matchesNone = findQueryMatches(index, "none");
    EXPECT(matchesNone.isEmpty());
}


STUDENT_TEST("findQueryMatches from tiny.txt, compound queries not found") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> matchesHelloOrThere = findQueryMatches(index, "hello there");
    EXPECT_EQUAL(matchesHelloOrThere.size(), 0);

    Set<string> matchesRedAndBread = findQueryMatches(index, "red +bread");
    EXPECT_EQUAL(matchesRedAndBread.size(), 0);

    Set<string> matchesGreenWithoutBlue = findQueryMatches(index, "green -blue");
    EXPECT_EQUAL(matchesGreenWithoutBlue.size(), 0);

}

STUDENT_TEST("findQueryMatches from tiny.txt, compound queries with duplicates") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> matchesRedOrRed = findQueryMatches(index, "red red");
    EXPECT_EQUAL(matchesRedOrRed.size(), 2);

    Set<string> matchesRedAndRed = findQueryMatches(index, "red +red");
    EXPECT_EQUAL(matchesRedAndRed.size(), 2);

    Set<string> matchesRedWithoutRed = findQueryMatches(index, "red -red");
    EXPECT_EQUAL(matchesRedWithoutRed.size(), 0);

}
