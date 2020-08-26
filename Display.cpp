//
// Created by Ryan McKay on 8/25/20.
//
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>
#include <algorithm>
#include "Display.h"

Display::Display() : display(-1) {}

void Display::HomeScreen() {
    display.clearDisplay();  // start with a blank display

    // Display Text
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Hello world!");

    // print first note number to the screen:
    display.setCursor(0, 14);
    String note1 = numberToNote(noteNum1);
    display.print(note1);

    // print the second note number the screen:
    display.setCursor(60, 14);
    String note2 = numberToNote(noteNum2);
    display.print(note2);

    display.display();  // send to screen
}

void Display::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

String Display::numberToNote(int number) {
    int octave = number / NOTES_IN_OCTAVE;
    String note;
    if (number >= 0 && number <= 127 && std::find(std::begin(OCTAVES), std::end(OCTAVES), octave) != std::end(OCTAVES)) {
        note = NOTES[number % NOTES_IN_OCTAVE];
//        note += std::to_string(octave);
        return note;
    }
    else note = "ERR";
    return note;
}