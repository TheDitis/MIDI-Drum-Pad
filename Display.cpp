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
    display.setTextSize(2);
    display.setTextColor(WHITE);
//    display.setCursor(0,0);
//    display.println("Hello world!");

    // print first note number to the screen:
    display.setCursor(noteNum1XLoc, 6);
    String note1 = numberToNote(noteNum1);
    display.print(note1);

    // print the second note number the screen:
    display.setCursor(noteNum2XLoc, 6);
    String note2 = numberToNote(noteNum2);
    display.print(note2);

    display.drawBitmap(settingsIconXLoc, 4, gearIcon2, 18, 18, WHITE);

    int underlineLocX = homeScreenSelection == 0 ? noteNum1XLoc : noteNum2XLoc;
//    display.drawLine(underlineLocX - 5, 28, underlineLocX + 20, 28, WHITE);
    display.drawRoundRect(noteNum1XLoc - 5, 3, 15 * note1.length(), 22, 5, WHITE);


    display.display();  // send to screen
}

void Display::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize the screen with the correct I2C address
}

String Display::numberToNote(int number) {
    int octave = number / NOTES_IN_OCTAVE;  // get the octave of the note
    String note;  // this will be the string returned representing the note and octave
    // if the number is between 0 and 127 (inclusive) and the octave is in the list of available octaves:
    if (number >= 0 && number <= 127 && std::find(std::begin(OCTAVES), std::end(OCTAVES), octave) != std::end(OCTAVES)) {
        note = NOTES[number % NOTES_IN_OCTAVE];  // get the note string without the octave
        note += String(octave);  // append the octave number
        return note;  // and return the resulting string
    }
    else note = "ERR";  // if conditions are not met, assign note to ERR
    return note;  // return value of note
}

int Display::getHomeMenuItemWidth(int index) {
//    int width;
//    if (index == 0) {
//        width = numberToNote(noteNum1).length() * CHAR_WIDTH_MED;
//    }
    switch (index) {
        case 0:
            return numberToNote(noteNum1).length() * CHAR_WIDTH_MED;
        case 1:
            return numberToNote(noteNum2).length() * CHAR_WIDTH_MED;
        case 3:
            return 20;
        default:
            Serial.println("ERROR: Index out of range in getHomeMenuItemWidth method of Display class")

    }
}