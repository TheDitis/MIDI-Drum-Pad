#include "XiaoPiezoDrum.h"
#include <iostream>
#include <Arduino.h>
#include <functional>
#include "RunningAverage.h"
#include "RotaryEncoder.h"
#include "RGBLED.h"
#include "Display.h"
#include <Math.h>

XiaoPiezoDrum::XiaoPiezoDrum() {
    Serial.begin(115200);  // start serial
    triggerBuffer = RunningAverage(200);  // initialize the buffer that we will use to collect velocity data once the threshold is crossed
}

XiaoPiezoDrum::XiaoPiezoDrum(int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin);
}

XiaoPiezoDrum::XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin, -1);  // initialize RGBLED, passing -1 to indicate that we are using a common anode LED
    LED.setFadeTarget(0, 255, 255, 1000);  // fade to the default blueish color
    sensor = PiezoSensor(piezoPin, 10);  // setup the Piezo sensor

}


void XiaoPiezoDrum::setNoteOnFunc(const function<void(int, int, int)>& onFunc) {
    comFunctionsSet = true;
    sendNoteHandler = onFunc;
}

void XiaoPiezoDrum::setNoteOffFunc(const function<void(int, int, int)>& offFunc) {
    comFunctionsSet = true;
    noteOffHandler = offFunc;
}

void XiaoPiezoDrum::RunCycle() {
    SensorReading = sensor.read();  // get the sensor value


    btnClicked = knob.wasClicked();
    knobState = knob.checkPosition();
    //knob.SerialPlotStates();

    if (btnClicked) handleButtonClick();
    // if the knob was turned in either direction, call handleKnobTurn passing in the direction indicator:
    if (knobState >= 0) handleKnobTurn();

    Serial.println(SensorReading);

    // see if conditions are right to send a note
    strikeDetected = SensorReading > THRESHOLD && !triggering && !triggered && !resting;

    // when we want to get ready to send a note:
    if (strikeDetected) TriggerHit();

    // when a hit was detected and we are gathering sensor data
    if (triggering) CalculateAndSendNote();

    // when a note is actively being sent and we're counting down to sending note-off signal:
    else if (triggered) NoteOffCountdown();

    // when we have just a note and are waiting our minimum amount of time between notes:
    else if (resting) DetermineRestingState();

    LED.update();
    SerialPlotStages();
}

void XiaoPiezoDrum::TriggerHit() {
    triggerStartTime = millis();  // set time sensor data collection started
    strikeDetected = false;  // make sure we don't run this block next loop, when we are in the middle of sampling
    triggering = true;  // signify that we are now going to start collecting sensor data before sending a note
    triggerBuffer.clear();  // clear sensor data collected from last strike if there is any
}

void XiaoPiezoDrum::CalculateAndSendNote() {
    int timeElapsed = millis() - triggerStartTime;
    if (timeElapsed < SAMPLE_TIME) triggerBuffer.addValue(SensorReading);  // if still in data collection timeframe:
    else {
        int triggerMax = round(triggerBuffer.getMax());
        rimshot = triggerMax > RIMSHOT_THRESH;
        PlayNoteNumber = rimshot ? NoteRim : Note;
        MaxVal = (!rimshot && triggerMax > MaxVal) ? triggerMax : MaxVal;  // if the value is greater than the current max, replace it
        MaxValRim = (rimshot && triggerMax > MaxValRim) ? triggerMax : MaxValRim;  // replace rimshot maxval if current val is greater
        // calculate velocity:
        int velocity = map(
                triggerMax, // the maximum value in the samples gathered from this strike
                rimshot ? RIMSHOT_THRESH : THRESHOLD,  // if it's a rimshot, the floor should be the threshold, 0 otherwise
                round(rimshot ? MaxValRim : MaxVal),  // use the corresponding maximum, rim or not
                1,  // minimum velocity for MIDI note
                127  // maximum velocity for MIDI note
        );
        if (onHomeScreen) LED.setFadeTarget(velocity * 2, 255 - (velocity * 2), 255 - (velocity * 2), 30);  // trigger led color change if you aren't in a menu
        if (comFunctionsSet) sendNoteHandler(PlayNoteNumber, velocity, 1);  // if midi noteOn handler has been provided, call it
        triggerSentTime = millis();  // time noteOn signal was sent, so we can calculate when to send noteOffHandler signal
        triggerStartTime = 0;
        triggering = false;  // signify that we are no longer gathering hit data
        triggered = true;  // signify that we are in the middle of sending a note
    }
}

void XiaoPiezoDrum::NoteOffCountdown() {
    // if the specified note length has passed:
    if (millis() - triggerSentTime >= NOTE_LENGTH) {
        if (onHomeScreen) LED.setFadeTarget(0, 255, 255, 100);  // return to default color if not in a menu
        if (comFunctionsSet) noteOffHandler(PlayNoteNumber, 0, 1);  // run noteOffHandler MIDI handler if it has been provided
        triggered = false;  //  signify that we have sent noteOffHandler signal
        resting = true;  // signify that we just finished sending a note and will wait REST_TIME ms before the next
        noteEndTime = millis();  // time noteOffHandler signal was sent to calculate when we can send again
    }
}

void XiaoPiezoDrum::DetermineRestingState() {
    // if the specified rest time has passed:
    if (millis() - noteEndTime >= REST_TIME && SensorReading <= OFF_THRESH) {
        resting = false;  // signify that we are done resting and can send another signal
        noteEndTime = 0;  // reset noteEndTime
    }
}

void XiaoPiezoDrum::SerialPlotStages() {
    int isTriggering, isTriggered, isResting;  // used to plot different stages
    // assign to high value if we are currently in that stage
    isTriggering = triggering ? 200 : 0;
    isTriggered = triggered ? 220 : 0;  // using a different value to make boundaries between stages more clear
    isResting = resting ? 200 : 0;

    // print out values with labels in proper format for arduino serial plotter:
    Serial.print("triggering: ");
    Serial.print(isTriggering);
    Serial.print("\t");
    Serial.print("triggered: ");
    Serial.print(isTriggered);
    Serial.print("\t");
    Serial.print("resting: ");
    Serial.print(isResting);
    Serial.print("\t");
    Serial.print("value: ");
    Serial.print(SensorReading);
    Serial.print("\t");
    Serial.println();
}

void XiaoPiezoDrum::setKnobPins(int pin1, int pin2, int clkPin) {
    knob = RotaryEncoder(pin1, pin2, clkPin);  // initialize the rotary encoder
}

void XiaoPiezoDrum::initDisplay() {
    display.init();  // start up the screen

    // pass the current note values:
    display.setNote1(Note);
    display.setNote2(NoteRim);

    display.HomeScreen();// display the home screen
}

void XiaoPiezoDrum::handleKnobTurn() {
    // if we are in selection mode rather than value editing mode:
    if (menuScrolling) {
        // if we are on the home screen:
        if (onHomeScreen) {
            // if the knob turned right, add one to the note
            int itemSelected = homeScreenSelection + ((knobState == 0) ? -1 : 1);
            if (itemSelected < 0) itemSelected = NUM_HOMESCREEN_ITEMS - 1;
            homeScreenSelection = itemSelected % NUM_HOMESCREEN_ITEMS;
            display.setHomeScreenSelection(homeScreenSelection);
        }
        else if (onSettingsScreen) {

        }
    }
    else if (changeMenuValue) {
        if (onHomeScreen) {
            changeRimshotNote = (homeScreenSelection == 1) ? true : false;
            // if we are not in the mode to change the rimshot note:
            if (!changeRimshotNote) {
                // change the primary note number by one
                Note += (knobState == 1) ? 1 : -1;  // if it was turned right, add 1, if left, subtract 1
                display.setNote1(Note);
            }
                // if we are in the mode to change the rimshot note:
            else {
                NoteRim += (knobState == 1) ? 1 : -1;  // change that note depending on the direction of the turn
                display.setNote2(NoteRim);
            }
        }
    }
    if (onHomeScreen) display.HomeScreen();
    else display.SettingsScreen();
}

void XiaoPiezoDrum::handleButtonClick() {
    if (onHomeScreen) {
        if (homeScreenSelection != NUM_HOMESCREEN_ITEMS - 1) {
            toggleMenuSelect();
            display.setEditingValue(changeMenuValue);
            display.HomeScreen();
        }
        else {
            toggleSettingsScreen();
        }
    }
    else {
        if (settingsSelection == 0) {
            toggleSettingsScreen();
        }
        else {
            display.SettingsScreen();
        }
    }
}

// to switch back and forth between the Home and Settings screens
void XiaoPiezoDrum::toggleSettingsScreen() {
    onHomeScreen = !onHomeScreen;
    onSettingsScreen = !onSettingsScreen;
    if (onHomeScreen) display.HomeScreen();
    else if (onSettingsScreen) display.SettingsScreen();
}