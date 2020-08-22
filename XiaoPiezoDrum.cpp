#include "XiaoPiezoDrum.h"
#include <iostream>
#include <Arduino.h>
#include <functional>
#include "RunningAverage.h"
#include "RGBLED.h"
#include <Math.h>


XiaoPiezoDrum::XiaoPiezoDrum() {
    Serial.begin(115200);
//    Serial.println("MIDI Initialized.");
//    while( !USBDevice.mounted() ) {
//        Serial.println("Connecting");
//        delay(10);
//    }
}

XiaoPiezoDrum::XiaoPiezoDrum(int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
//    Serial.begin(115200);
    LED = RGBLED(rPin, gPin, bPin);
//    LED.fadeTo(255, 0, 0, 1000);
}

XiaoPiezoDrum::XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin) : XiaoPiezoDrum() {
//    Serial.begin(115200);
    LED = RGBLED(rPin, gPin, bPin, -1);
//    LED.setColor(0, 0, 0);
    LED.fadeTo(255, 0, 0, 1000);
    sensor = PiezoSensor(piezoPin, 10);
    triggerBuffer = RunningAverage(200);
}

//void XiaoPiezoDrum::setNoteComFunctions(const std::function<void(int, int, int)>& onFunc,
//                                        const std::function<void(int, int, int)>& offFunc) {
////    using namespace std;
////    using namespace placeholders;
////    sendNote = [onFunc](int && PH1, int && PH2, int && PH3) {
////        Serial.printf("Sending note! %d, %d, %d", PH1, PH2, PH3);
////        return onFunc(PH1, PH2, PH3);
////    };
//    sendNote = [onFunc](int && PH1, int && PH2, int && PH3) { onFunc(PH1, PH2, PH3); };
////    noteOff = [offFunc](int && PH1, int && PH2, int && PH3) {
////        Serial.printf("Sending note! %d, %d, %d", PH1, PH2, PH3);
//////        return offFunc(PH1, PH2, PH3);
////    };
//
//    comFunctionsSet = true;
////    noteOff = bind(offFunc, _1, _2, _3);
//}

//void XiaoPiezoDrum::setNoteComFunctions() {
//    comFunctionsSet = true;
//    sendNote = [](int n, int v, int c) {Serial.printf("Note: %d %d %d", n, v, c);};
//}

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
    double val = sensor.read();  // get the sensor value
    val = (val > 0) ? val : 0;  // make sure it's not negative
    MaxVal = (val > MaxVal) ? val : MaxVal;  // if the value is greater than the current max, replace it
    // set to trigger if val is over threshold, it's not already triggering, and it isn't resting after last trigger
    trigger = val > THRESHOLD && !triggering && !triggered && !resting;

//    if (val > THRESHOLD && !triggering) trigger = true;
    if (trigger) {
        triggerStartTime = millis();
        trigger = false;
        triggering = true;
        triggerBuffer.clear();
        Serial.println(val);
        Serial.printf("sampling %d\n", 200);
    }
    else if (triggering) {
        timeElapsed = millis() - triggerStartTime;
        if (timeElapsed < SAMPLE_TIME) {
            loopCounter++;
            triggerBuffer.addValue(val);
            Serial.println(val);
            Serial.printf("sampling %d\n", 200);
        }
        else {
            Serial.printf("sampling %d\n", 0);
            Serial.println(val);
            triggerMax = round(triggerBuffer.getMax());
            velocity = map(triggerMax, 0, round(MaxVal), THRESHOLD, 127);
            if (comFunctionsSet) sendNote(Note, velocity, 1);  // if midi noteOn handler has been provided, call it
            triggerSentTime = millis();  // time noteOn signal was sent, so we can calculate when to send noteOff signal
            triggerStartTime = 0;
            triggerBuffer.clear();  // get rid of all the data from this hit
            triggering = false;  // signify that we are no longer gathering hit data
            triggered = true;  // signify that we are in the middle of sending a note
        }
    }

    // when a note is actively being sent:
    else if (triggered) {
        // if the specified note length has passed:
        if (millis() - triggerSentTime >= NOTE_LENGTH) {
            if (comFunctionsSet) noteOff(Note, 0, 1);  // run noteOff MIDI handler if it has been provided
            triggered = false;  //  signify that we have sent noteOff signal
            resting = true;  // signify that we just finished sending a note and will wait REST_TIME ms before the next
            noteEndTime = millis();  // signify time noteOff signal was sent to calculate when we can send again
        }
        Serial.printf("sampling %d\n", 0);
        Serial.println(val);

    }

    // when we have just a note and are waiting our minimum amount of time between notes:
    else if (resting) {
        // if the specified rest time has passed:
        if (millis() - noteEndTime >= REST_TIME && val <= OFF_THRESH) {
            resting = false;  // signify that we are done resting and can send another signal
            noteEndTime = 0;  // reset noteEndTime
        }
        Serial.printf("sampling %d\n", 0);
        Serial.println(val);
    }
    else {
        Serial.printf("sampling %d\n", 0);
        Serial.println(val);
    }
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
