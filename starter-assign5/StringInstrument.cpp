/*
 * this file contains the code implementing the StringInstrument class
 * using the Karplus-Strong rules to simulate the vibration of a string instrument,
 * alongside associated tests to confirm validity of functions
 */

#include "Demos/AudioSystem.h"
#include "StringInstrument.h"
#include "error.h"
using namespace std;

/* This function constructs StringInstrument, tuned to the frequency
 * given as the only parameter. This is done with the Karpus-Strong
 * algorithm, which sets the pitch according to the waveform length
 */
StringInstrument::StringInstrument(double frequency) {

    // give an error if the frequency is negative or 0
    if (frequency <= 0)
        error("Frequency is not positive");

    // the length of the waveform is the sample rate divided by frequency
    _length = AudioSystem::sampleRate()/frequency;

    // the waveform must contain at least 2 samples to be valid
    if (_length < 2)
        error("Array must be longer than 1 value");

    // initiate the waveform
    _waveform = new Sample[_length];

    // iterate through the waveform, initializing to 0
    for (int i = 0; i < _length; i++)
        _waveform[i] = 0.0;

    _cursor = 0;
}

/* This function is the destructor, which releases memory by
 * deleting the saved waveform to prevent memory leaks
 */
StringInstrument::~StringInstrument() {
    delete[] _waveform;
}

/* This function simulates the plucking of the string and initializes
 * the waveform to a square wave, using 0.05 as the amplitude for
 * the wave. After the "pluck," the cursor is reset so the sound
 * can start from the beginning again
 */
void StringInstrument::pluck() {
    // iterate through the wave to initialize the wavelength
    for (int i = 0; i < _length; i++) {
        _waveform[i] = 0.05;
        // if we're at the halfway mark or after, the wavelength is negative
        if (i >= _length/2)
            _waveform[i] *= -1;
    }

    _cursor = 0;
}

/* This funtion starts the next time step for the wave simulation.
 * This is doen with the karplus-strong rule. The function returns
 * the current sample that is processed, and updates the waveform accordingly.
 */
Sample StringInstrument::nextSample() {

    // current sample
    double cur = _waveform[_cursor];

    // determine the index of the next sample
    int idx;
    // if the current sample is already at the end of the wave, reset to the beginning
    if (_cursor == _length-1)
        idx = 0;
    else idx = _cursor+1;

    double next = _waveform[idx];

    // update to the next sample using the karplus-strong formula
    _waveform[_cursor] = 0.995*(cur+next)/2.0;

    // move the cursor to the next index as calculated above
    _cursor = idx;

    // return current sample
    return cur;
}

/* * * * * Test Cases Below This Point * * * * */

#include "GUI/SimpleTest.h"

PROVIDED_TEST("Milestone 2: Waveform array initialized correctly.") {
    /* Change the sample rate to 3, just to make the numbers come out nice. */
    AudioSystem::setSampleRate(3);

    /* Create a string that vibrates at 1hz. This is well below the human hearing
     * threshold and exists purely for testing purposes.
     */
    StringInstrument instrument(1);

    /* Make sure something was allocated. */
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);

    /* Length should be 3 / 1 = 3. */
    EXPECT_EQUAL(instrument._length, 3);

    /* All entries should be zero. */
    EXPECT_EQUAL(instrument._waveform[0], 0);
    EXPECT_EQUAL(instrument._waveform[1], 0);
    EXPECT_EQUAL(instrument._waveform[2], 0);
}

PROVIDED_TEST("Milestone 2: Constructor reports errors on bad inputs.") {
    /* To make the math easier. */
    AudioSystem::setSampleRate(10);

    EXPECT_ERROR(StringInstrument error(-1));  // Negative frequency
    EXPECT_ERROR(StringInstrument error(0));   // Zero frequency
    EXPECT_ERROR(StringInstrument error(10));  // Array would have length 1
    EXPECT_ERROR(StringInstrument error(100)); // Array would have length 0

    /* But we shouldn't get errors for good values. */
    StringInstrument peachyKeen(1);
    EXPECT_NOT_EQUAL(peachyKeen._waveform, nullptr);
}

PROVIDED_TEST("Milestone 2: Constructor sets cursor to position 0.") {
    AudioSystem::setSampleRate(10);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);

    EXPECT_EQUAL(instrument._cursor, 0);
}

PROVIDED_TEST("Milestone 3: pluck does not allocate a new array.") {
    AudioSystem::setSampleRate(10);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);

    /* Plucking the string should change the contents of the array, but not
     * which array we're pointing at.
     */
    Sample* oldArray = instrument._waveform;
    instrument.pluck();

    EXPECT_EQUAL(instrument._waveform, oldArray);
}

PROVIDED_TEST("Milestone 3: pluck sets values to +0.05 and -0.05.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);
    EXPECT_EQUAL(instrument._length, 4);

    instrument.pluck();
    EXPECT_EQUAL(instrument._waveform[0], +0.05);
    EXPECT_EQUAL(instrument._waveform[1], +0.05);
    EXPECT_EQUAL(instrument._waveform[2], -0.05);
    EXPECT_EQUAL(instrument._waveform[3], -0.05);
}

PROVIDED_TEST("Milestone 3: pluck resets the cursor.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);

    /* Invasively move the cursor forward. This is called an "invasive"
     * test because it manipulates internal state of the type we're
     * testing, rather than just using the interface.
     */
    instrument._cursor = 3;
    instrument.pluck();
    EXPECT_EQUAL(instrument._cursor, 0);
}

PROVIDED_TEST("Milestone 4: nextSample works if pluck not called.") {
    AudioSystem::setSampleRate(10);

    /* 10 samples per sec / 1Hz = 10 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);
    EXPECT_EQUAL(instrument._length, 10);

    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument._cursor, 4);
}

PROVIDED_TEST("Milestone 4: nextSample updates waveform array.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);
    EXPECT_EQUAL(instrument._length, 4);

    instrument.pluck();
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.nextSample(), -0.05);
    EXPECT_EQUAL(instrument._cursor, 3);

    /* The first array value is the average of +0.05 and +0.05, scaled by 0.995.
     * The two values are the same, so we should get back +0.05 scalled by 0.995.
     */
    EXPECT_EQUAL(instrument._waveform[0], +0.05 * 0.995);

    /* The next array value is the average of +0.05 and -0.05, scaled by 0.995.
     * This is exactly zero.
     */
    EXPECT_EQUAL(instrument._waveform[1], 0.0);

    /* The next array value is the average of -0.05 and -0.05, scaled by 0.995.
     * As with the first entry, this is -0.05 scaled by 0.995.
     */
    EXPECT_EQUAL(instrument._waveform[2], -0.05 * 0.995);
}

PROVIDED_TEST("Milestone 4: nextSample wraps around properly.") {
    AudioSystem::setSampleRate(2);

    /* 2 samples per sec / 1Hz = 2 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument._waveform, nullptr);
    EXPECT_EQUAL(instrument._length, 2);

    /* Pluck the string, forming the array [+0.05, -0.05] */
    instrument.pluck();

    /* Read two samples, which should be +0.05 and -0.05. */
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument._cursor, 1);
    EXPECT_EQUAL(instrument.nextSample(), -0.05);
    EXPECT_EQUAL(instrument._cursor, 0);

    /* The first array value is the average of +0.05 and -0.05, scaled by 0.995.
     * This is zero.
     */
    EXPECT_EQUAL(instrument._waveform[0], 0.0);

    /* The next array value is the average of -0.05 and 0, scaled by 0.995. */
    Sample decayedTerm = 0.995 * (-0.05 + 0) / 2.0;
    EXPECT_EQUAL(instrument._waveform[1], decayedTerm);

    /* Get two more samples. The waveform is [0, decayedTerm], so we should
     * get back 0, then decayedTerm.
     */
    EXPECT_EQUAL(instrument.nextSample(), 0.0);
    EXPECT_EQUAL(instrument._cursor, 1);
    EXPECT_EQUAL(instrument.nextSample(), decayedTerm);
    EXPECT_EQUAL(instrument._cursor, 0);

    /* The first array value is the average of 0.0 and decayedTerm, scaled by
     * 0.995.
     */
    Sample moreDecayed = 0.995 * (decayedTerm + 0) / 2.0;
    EXPECT_EQUAL(instrument._waveform[0], moreDecayed);

    /* The second array value is the average of decayedTerm and moreDecayed,
     * scaled by 0.995.
     */
    EXPECT_EQUAL(instrument._waveform[1], 0.995 * (decayedTerm + moreDecayed) / 2.0);
}

// student tests for milestones

STUDENT_TEST("Milestone 2: check rounding down for length") {
    AudioSystem::setSampleRate(10);
    StringInstrument instrument(3);

    // the waveform length should be rounded down
    EXPECT_EQUAL(instrument._length, 3);
}

STUDENT_TEST("Milestone 2: boundary condition of length 2 passes") {
    AudioSystem::setSampleRate(10);
    StringInstrument instrument(5);

    // check that length 2 is valid
    EXPECT_EQUAL(instrument._length, 2);
    // check that waveform is initialized to all 0s
    EXPECT_EQUAL(instrument._waveform[0], 0);
    EXPECT_EQUAL(instrument._waveform[1], 0);
    // check that cursor starts at 0
    EXPECT_EQUAL(instrument._cursor, 0);

    // expect error for length of 1
    EXPECT_ERROR(StringInstrument bad(10));
}

STUDENT_TEST("Milestone 3: check all values in odd numbered array are filled") {
    AudioSystem::setSampleRate(4);
    StringInstrument instrument(2);

    instrument.pluck();

    // check conditions after pluck - waveform length and cursor reset
    EXPECT_EQUAL(instrument._length, 2);
    EXPECT_EQUAL(instrument._cursor, 0);

    // check that first half is initialized to positive 0.05
    EXPECT_EQUAL(instrument._waveform[0], 0.05);
    // second half is negative 0.05
    EXPECT_EQUAL(instrument._waveform[1], -0.05);
}

STUDENT_TEST("Milestone 3: check all values in odd numbered array are filled") {
    AudioSystem::setSampleRate(9);
    StringInstrument instrument(3);

    instrument.pluck();

    // check conditions after pluck - waveform length and cursor reset
    EXPECT_EQUAL(instrument._length, 3);
    EXPECT_EQUAL(instrument._cursor, 0);

    // first half is initialized to positive 0.05
    EXPECT_EQUAL(instrument._waveform[0], 0.05);
    // second half is negative 0.05, including middle value for odd numbered arrays
    EXPECT_EQUAL(instrument._waveform[1], -0.05);
    EXPECT_EQUAL(instrument._waveform[2], -0.05);
}

STUDENT_TEST("Milestone 4: updates first waveform correctly") {
    AudioSystem::setSampleRate(5);

    StringInstrument instrument(1);
    instrument.pluck();

    // check that after pluck, karplus-strong rule holds true
    double next = instrument.nextSample();
    EXPECT_EQUAL(next, 0.05);
    EXPECT_EQUAL(instrument._waveform[0], 0.995*0.05);
    EXPECT_EQUAL(instrument._cursor, 1);
}

STUDENT_TEST("Milestone 4: test sign is correct when averaging middle") {
    AudioSystem::setSampleRate(6);

    StringInstrument instrument(1);
    instrument.pluck();

    // check that two steps into the waveform, correctly averages out middle value
    for (int i = 0; i < 2; i++)
        instrument.nextSample();

    EXPECT_EQUAL(instrument.nextSample(), 0.05);
    EXPECT_EQUAL(instrument._waveform[2], 0);
}


/* * * * * Special Functions Below This Point * * * * */

#include <algorithm>

/* Copy constructor for StringInstrument. This is called automatically by C++
 * if you need to make a copy of a StringInstrument; say, if you were to pass
 * one into a function by value. Take CS106L for more details about how this
 * works.
 */
StringInstrument::StringInstrument(const StringInstrument& rhs) {
    /* Not normally part of a copy constructor, this line is designed
     * to ensure that if you haven't implemented StringInstrument,
     * you don't get weird and wild crashes.
     */
    if (rhs._waveform == nullptr) {
        return;
    }

    _length   = rhs._length;
    _waveform = new Sample[_length];
    _cursor   = rhs._cursor;

    for (int i = 0; i < _length; i++) {
        _waveform[i] = rhs._waveform[i];
    }
}

/* Assignment operator for StringInstrument. This is called automatically by
 * C++ when you assign one StringInstrument to another. Take CS106L for more
 * details about how this works. This specific implementation uses an idiom
 * called "copy-and-swap."
 */
StringInstrument& StringInstrument::operator =(StringInstrument rhs) {
    swap(_length,   rhs._length);
    swap(_waveform, rhs._waveform);
    swap(_cursor,   rhs._cursor);
    return *this;
}
