#include "XiaoPiezoDrum.h"
#include <iostream>
#include <Arduino.h>
#include <functional>
#include "RunningAverage.h"
#include "RotaryEncoder.h"
#include "RGBLED.h"
#include <Math.h>
using namespace std::placeholders;

XiaoPiezoDrum::XiaoPiezoDrum() {
    Serial.begin(115200);
    triggerBuffer = RunningAverage(200);
}

XiaoPiezoDrum::XiaoPiezoDrum(int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin);
}

XiaoPiezoDrum::XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin, -1);
    LED.fadeTo(0, 255, 255, 1000);
    sensor = PiezoSensor(piezoPin, 10);
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
    int turnState;
    SensorReading = sensor.read();  // get the sensor value

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
//    SerialPlotStages();
    knob.isClicked();
    turnState = knob.checkPosition();
//    knob.SerialPlotStates();

    if (turnState >= 0)
        Note = (turnState == 1) ? Note + 1 : Note - 1;
//    Serial.print("Note:");
//    Serial.print(Note);
//    Serial.print("\t");
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
        int velocity = map(
                triggerMax, // the maximum value in the samples gathered from this strike
                rimshot ? RIMSHOT_THRESH : THRESHOLD,  // if it's a rimshot, the floor should be the threshold, 0 otherwise
                round(rimshot ? MaxValRim : MaxVal),  // use the corresponding maximum, rim or not
                1,  // minimum velocity for MIDI note
                127  // maximum velocity for MIDI note
        );
        LED.setFadeTarget(velocity * 2, 255 - (velocity * 2), 255 - (velocity * 2), 30);
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
        LED.setFadeTarget(0, 255, 255, 100);
        if (comFunctionsSet) noteOffHandler(PlayNoteNumber, 0, 1);  // run noteOffHandler MIDI handler if it has been provided
        triggered = false;  //  signify that we have sent noteOffHandler signal
        resting = true;  // signify that we just finished sending a note and will wait REST_TIME ms before the next
        noteEndTime = millis();  // signify time noteOffHandler signal was sent to calculate when we can send again
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
    int isTriggering, isTriggered, isResting;
    isTriggering = triggering ? 200 : 0;
    isTriggered = triggered ? 220 : 0;
    isResting = resting ? 200 : 0;

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
    knob = RotaryEncoder(pin1, pin2, clkPin);
}