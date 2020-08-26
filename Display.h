//
// Created by Ryan McKay on 8/25/20.
//

#ifndef XIAOPIEZODRUM_DISPLAY_H
#define XIAOPIEZODRUM_DISPLAY_H

#include <string>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NOTES_IN_OCTAVE 12



class Display {

public:
    Display();
    void HomeScreen();
    void init();
    String numberToNote(int number);
    void setNoteNum1(int noteNum);
    void setNoteNum2(int noteNum);

private:
    int noteNum1, noteNum2;
    Adafruit_SSD1306 display;
    String NOTES[NOTES_IN_OCTAVE] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int OCTAVES[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
};

/*
 * def number_to_note(number: int) -> tuple:
    octave = number // NOTES_IN_OCTAVE
    assert octave in OCTAVES, errors['notes']
    assert 0 <= number <= 127, errors['notes']
    note = NOTES[number % NOTES_IN_OCTAVE]

    return note, octave
 */


#endif //XIAOPIEZODRUM_DISPLAY_H
