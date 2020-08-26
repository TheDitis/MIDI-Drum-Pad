//
// Created by Ryan McKay on 8/25/20.
//

#ifndef XIAOPIEZODRUM_DISPLAY_H
#define XIAOPIEZODRUM_DISPLAY_H

#include <string>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NOTES_IN_OCTAVE 12
#define CHAR_WIDTH_MED 15

const unsigned char gearIcon [] PROGMEM = {
        0x00, 0x00, 0x07, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 0x3f, 0xe0, 0x78, 0xf0, 0x78, 0xf0, 0x78, 0xf0,
        0x3f, 0xe0, 0x3f, 0xf0, 0x3f, 0xf0, 0x07, 0x00
};

// 'gearIcon18', 18x18px
const unsigned char gearIcon2 [] PROGMEM = {
        0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x0d, 0xe6, 0x00, 0x3f, 0xff, 0x00, 0x3f, 0xff, 0x00, 0x3f,
        0xff, 0x00, 0x1f, 0x3e, 0x00, 0x7e, 0x1f, 0x80, 0x7c, 0x0f, 0x80, 0x7c, 0x0f, 0x80, 0x7e, 0x1f,
        0x80, 0x1f, 0x3e, 0x00, 0x3f, 0xff, 0x00, 0x3f, 0xff, 0x00, 0x3f, 0xff, 0x00, 0x0d, 0xe6, 0x00,
        0x01, 0xe0, 0x00, 0x00, 0x00, 0x00
};



class Display {

public:
    Display();
    void HomeScreen();  // the method that displays the homescreen
    void init();
    String numberToNote(int number);
    void setNote1(int noteNum) { noteNum1 = noteNum;}
    void setNote2(int noteNum) { noteNum2 = noteNum;}

private:
    Adafruit_SSD1306 display;  // Adafruit display object that we will use to write values to the screen
    int noteNum1, noteNum2;  // the numbers representing the two different notes (primary and rimshot)
    int noteNum1XLoc = 6;
    int noteNum2XLoc = 55;
    int settingsIconXLoc = 105;
    String NOTES[NOTES_IN_OCTAVE] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};  // possible note options
    int OCTAVES[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};  // octaves that work with midi
    int homeScreenSelection = 0;  // this will determine which item is selected for editing
    int settingsSelection = 0;
    int homeMenuItemLocations[3] = {noteNum1XLoc, noteNum2XLoc, settingsIconXLoc};
    int getHomeMenuItemWidth(int index);
    // 'gearicon', 12x12px


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
