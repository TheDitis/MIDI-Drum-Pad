#include "XiaoPiezoDrum.h"
#include <iostream>
#include <Arduino.h>
#include <functional>
#include "RunningAverage.h"
#include "RGBLED.h"
#include <Math.h>


XiaoPiezoDrum::XiaoPiezoDrum() {
    Serial.begin(115200);
    triggerBuffer = RunningAverage(200);
//    Serial.println("MIDI Initialized.");
//    while( !USBDevice.mounted() ) {
//        Serial.println("Connecting");
//        delay(1);
//    }
}

XiaoPiezoDrum::XiaoPiezoDrum(int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin);
}

XiaoPiezoDrum::XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
    LED = RGBLED(rPin, gPin, bPin, -1);
    LED.fadeTo(255, 0, 0, 1000);
    sensor = PiezoSensor(piezoPin, 10);
}


void XiaoPiezoDrum::setNoteOnFunc(const function<void(int, int, int)>& onFunc) {
    comFunctionsSet = true;
    sendNote = onFunc;
}

void XiaoPiezoDrum::setNoteOffFunc(const function<void(int, int, int)>& offFunc) {
    comFunctionsSet = true;
    noteOff = offFunc;
}

void XiaoPiezoDrum::RunCycle() {
    unsigned long timeElapsed;
    int triggerMax;
    int velocity;
    int isTriggering, isTriggered, isResting = 0;


    double val = sensor.read();  // get the sensor value

    // set to trigger if val is over threshold, it's not already triggering, and it isn't resting after last trigger
    trigger = val > THRESHOLD && !triggering && !triggered && !resting;

    // when we want to get ready to send a note:
    if (trigger) {
        triggerStartTime = millis();  // set time sensor data collection started
        trigger = false;  // make sure we don't run this block next loop, when we are in the middle of sampling
        triggering = true;  // signify that we are now going to start collecting sensor data before sending a note
        triggerBuffer.clear();  // clear sensor data collected from last strike if there is any
    }
    // when a hit was detected and we are gathering sensor data
    if (triggering) {
        timeElapsed = millis() - triggerStartTime;
        if (timeElapsed < SAMPLE_TIME) triggerBuffer.addValue(val);  // if still in data collection timeframe:
        else {
            triggerMax = round(triggerBuffer.getMax());
            rimshot = triggerMax > RIMSHOT_THRESH;
            PlayNoteNumber = rimshot ? NoteRim : Note;
            MaxVal = (!rimshot && val > MaxVal) ? val : MaxVal;  // if the value is greater than the current max, replace it
            MaxValRim = (rimshot && val > MaxValRim) ? val : MaxValRim;  // replace rimshot maxval if current val is greater
            velocity = map(
                    triggerMax, // the maximum value in the samples gathered from this strike
                    rimshot ? RIMSHOT_THRESH : THRESHOLD,  // if it's a rimshot, the floor should be the threshold, 0 otherwise
                    round(rimshot ? MaxValRim : MaxVal),  // use the corresponding maximum, rim or not
                    1,  // minimum velocity for MIDI note
                    127  // maximum velocity for MIDI note
                    );
            if (comFunctionsSet) sendNote(PlayNoteNumber, velocity, 1);  // if midi noteOn handler has been provided, call it
            triggerSentTime = millis();  // time noteOn signal was sent, so we can calculate when to send noteOff signal
            triggerStartTime = 0;
            triggering = false;  // signify that we are no longer gathering hit data
            triggered = true;  // signify that we are in the middle of sending a note
        }
    }

    // when a note is actively being sent:
    else if (triggered) {
        // if the specified note length has passed:
        if (millis() - triggerSentTime >= NOTE_LENGTH) {
            if (comFunctionsSet) noteOff(PlayNoteNumber, 0, 1);  // run noteOff MIDI handler if it has been provided
            triggered = false;  //  signify that we have sent noteOff signal
            resting = true;  // signify that we just finished sending a note and will wait REST_TIME ms before the next
            noteEndTime = millis();  // signify time noteOff signal was sent to calculate when we can send again
        }

    }

    // when we have just a note and are waiting our minimum amount of time between notes:
    else if (resting) {
        // if the specified rest time has passed:
        if (millis() - noteEndTime >= REST_TIME && val <= OFF_THRESH) {
            resting = false;  // signify that we are done resting and can send another signal
            noteEndTime = 0;  // reset noteEndTime
        }
    }
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
    Serial.print(val);
    Serial.println();
}

//
//void handleNoteOn(int channel, int pitch, int velocity) {
//    // Log when a note is pressed.
//    Serial.printf("Note on: channel = %d, pitch = %d, velocity - %d", channel, pitch, velocity);
//    Serial.println();
//}
//
//void handleNoteOff(int channel, int pitch, int velocity) {
//    // Log when a note is released.
//    Serial.printf("Note off: channel = %d, pitch = %d, velocity - %d", channel, pitch, velocity);
//    Serial.println();
//}
