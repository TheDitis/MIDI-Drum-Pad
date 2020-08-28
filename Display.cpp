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

    // Setup defaults:
    display.setTextSize(2);
    display.setTextColor(WHITE);

    // Printing the selection rectangle:
    // if editing a value, make the selection rectangle filled in:
    if (editingValue) display.fillRoundRect(homeMenuItemLocations[homeScreenSelection] - 5, 0, getHomeMenuItemWidth(homeScreenSelection), 26, 9, WHITE);
    // otherwise just make the border of the rectangle:
    else display.drawRoundRect(homeMenuItemLocations[homeScreenSelection] - 5, 0, getHomeMenuItemWidth(homeScreenSelection), 26, 9, WHITE);

    // printing the first home screen item
    // if the first item is selected and we are in value-editing mode, make the text black, otherwise make it white:
    display.setTextColor(editingValue && homeScreenSelection == 0 ? BLACK : WHITE);
    display.setCursor(noteNum1XLoc, 6);  // set the cursor to the position of the first item
    String note1 = numberToNote(noteNum1);  // get the string representation of the primary note number
    display.print(note1);  // print that string to the screen

    // printing the second home screen item:
    // if the second item is selected and we are in value-editing mode, make the text black, otherwise make it white:
    display.setTextColor(editingValue && homeScreenSelection == 1 ? BLACK : WHITE);
    display.setCursor(noteNum2XLoc, 6);  // set the cursor to the position of the second item
    String note2 = numberToNote(noteNum2);  // get the string representation of the secondary (rimshot) note
    display.print(note2);  // print that string to the screen

    // display settings gear icon:
    display.drawBitmap(settingsIconXLoc, 4, gearIcon2, 18, 18, WHITE);

    display.display();  // send to screen
}

void Display::SettingsScreen() {
    display.clearDisplay(); // erase screen

    // Setup defaults:
    display.setTextSize(1);
    display.setTextColor(WHITE);

//    display.setCursor(0, 0);
    drawSelectionArrow(0, 0);
    display.setCursor(10, 0);
    display.print("Home");

    display.display();
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
        note += String(octave-2);  // append the octave number
        return note;  // and return the resulting string
    }
    else note = "ERR";  // if conditions are not met, assign note to ERR
    return note;  // return value of note
}

int Display::getHomeMenuItemWidth(int index) {
    switch (index) {
        case 0:
            return numberToNote(noteNum1).length() * CHAR_WIDTH_MED;
        case 1:
            return numberToNote(noteNum2).length() * CHAR_WIDTH_MED;
        case 2:
            return 27;
        default:
            Serial.println("ERROR: Index out of range in getHomeMenuItemWidth method of Display class");
            break;
    }
}

int Display::getHomeScreenSelection() {
    return homeScreenSelection;
}

void Display::setHomeScreenSelection(int itemNum) {
//    if (itemNum < 0) itemNum = NUM_HOMESCREEN_ITEMS - itemNum;
//    homeScreenSelection = itemNum % NUM_HOMESCREEN_ITEMS;  // make sure we are in range
//    Serial.println(homeScreenSelection);
//    return homeScreenSelection;
    homeScreenSelection = itemNum;
}

void Display::drawSelectionArrow(int x, int y) {
    int height = 5;
    int width = 5;
    // Setup defaults:
    display.setTextSize(1);
    display.drawTriangle(x, y, x, y + height, x + width, y + (height / 2), WHITE);
}