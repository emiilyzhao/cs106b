/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <cctype>
#include <fstream>
#include <string>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* This function is intended to return a string which
 * includes only the letter characters from the original
 * (all non-letter characters are excluded)
 *
 * WARNING: The provided code is buggy!
 *
 * Use test cases to identify which inputs to this function
 * are incorrectly handled. Then, remove this comment and
 * replace it with a description of the bug you fixed.
 */


/* The codes vector stores subvectors of characters that correspond to the soundex
 * digits. The index of each subvector represents the soundex index that the letters
 * within the character correspond to.
 */
const vector<vector<char>> codes = {
    {'A', 'E', 'I', 'O', 'U', 'H', 'W', 'Y'},
    {'B', 'F', 'P', 'V'},
    {'C', 'G', 'J', 'K', 'Q', 'S', 'X', 'Z'},
    {'D', 'T'},
    {'L'},
    {'M', 'N'},
    {'R'}
};

/* The lettersOnly function takes one argument s and returns a string with only the
 * letter characters of s. All non-letter characters are removed by iterating through
 * s and then only appending each character to the result string if it is a letter
 * in the ASCII system.
 */
string lettersOnly(string s) {
    string result;
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

/* The encoded function takes one argument s, representing a name, and returns a string
 * of the name encoded under the Soundex characterization. The function first converts
 * s to upercase. Each character of s is iterated through, and then each sublist
 * in the soundex codes defined above, to check which sublist the character matches a
 * letter in. When the match is found, the sublist index is appended to the returned
 * result. To skip iterating through every substring, boolean found is defined
 * to be true, so that the iteration through the codes list is broken.
 */
string encoded(string s) {
    s = toUpperCase(s);
    string result;

    for (int i = 0; i < s.length(); i++) {
        bool found = false;
        for (int digitList = 0; digitList < codes.size(); digitList++) {
            for (int digit = 0; digit < codes[digitList].size(); digit++) {
                if (s[i] == codes[digitList][digit]) {
                    result += to_string(digitList);
                    found = true;
                    break;
                }
            }
            if (found)   break;
        }
    }
    return result;
}

/* The removeDuplicates function takes one argument s, representing a name, and returns
 * a string of the name with all duplicate characters removed. The function iterates
 * through each character in the name starting with the second, and initializes the
 * first character as char last. For each character, if it does not equal the last
 * character, it is be appended to the resulting string to be returned. The last
 * char is redefined after every char to be the current char.
 */
string removeDuplicates(string s) {
    string result;
    char last = s[0];

    for (int i = 1; i < s.size(); i++) {
        if (s[i] != last)   result += s[i];
        last = s[i];
    }

    return result;

}

/* The removeZeros function takes one argument s, representing an encoded name,
 * and returns a string of the name with all zeros removed. It iterates through
 * eavh character of the encoded name, and only appends it to the resulting string
 * to be returned if the character is not a zero.
 */
string removeZeros(string s) {
    string result;

    for (int i = 0; i < s.size(); i++) {
        if (s[i] != '0')   result += s[i];
    }

    return result;
}


/* The fixSize function takes one argument s, representing an encoded name, and
 * returns a string of the name as 4 characters. If the name is more than or equal
 * to 4 characters long, the function returns the first 4 characters. Otherwise, if
 * it is less than 4 characters long, the function adds zeros until it reaches 4
 * characters long.
 */
string fixSize(string s) {
    string result;
    if (s.size() >= 4)
        result.append(s, 0, 4);
    else {
        result = s;
        while(result.size() < 4)   result += "0";
    }
    return result;
}

/* The soundex function takes one argument s, representing a name, and returns the
 * name encoded using the soundex algorithm. The function first stores the first
 * character of the name in a variable, and then applies the steps of soundex using
 * the associated functions created above: encoding, removing duplicates,
 * replacing the first digit with uppercase letter, removing zeros, and finally fixing the size.
 */
string soundex(string s) {
    char first = s[0];
    string result = fixSize(removeZeros(toUpperCase(first) + removeDuplicates(encoded(s))));

    return result;
}



/* The soundexSearch function takes one argument filepath, containing a database of
 * names, and allows the user to enter a surname to look up in the database. Until
 * the user enters RETURN to quit and break the loop, the function prints the soundex
 * code of the name, as well as the associated soundex matches in the database.
 * For each name entered, the function iterates through the database and appending
 * all matches to a vector to be sorted and then returned.
 */
void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        allNames = readLines(in);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl;

    // The names read from file are now stored in Vector allNames

    while(true) {
        string name = getLine("\nEnter a surname (RETURN to quit): ");
        if (name.size() == 0)   break;

        string encodedName = soundex(name);
        cout << "Soundex code is " + encodedName;

        Vector<string> matches;
        for(int i = 0; i < allNames.size(); i++) {
            if (soundex(allNames[i]) == encodedName) {
                matches += allNames[i];
            }
        }
        matches.sort();
        cout << "\nMatches from database: " << matches << endl;
    }

    cout << "All done!" << endl;

}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}

STUDENT_TEST("Expose bug in lettersOnly: if the first digit is not a letter, it won't be removed") {
    EXPECT_EQUAL(lettersOnly("-e"), "e");
    EXPECT_EQUAL(lettersOnly(" e"), "e");
    EXPECT_EQUAL(lettersOnly(".e"), "e");
}
