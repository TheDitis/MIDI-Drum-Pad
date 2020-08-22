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

void XiaoPiezoDrum::setNoteComFunctions(const function<void(int, int, int)>& onFunc) {
    comFunctionsSet = true;
//    sendNote = [onFunc](auto && PH1, auto && PH2, auto && PH3) { onFunc(PH1, PH2, PH3); };
    sendNote = onFunc;
}


void XiaoPiezoDrum::RunCycle() {
    double val = sensor.read();
    val = (val > 0) ? val : 0;
    Serial.println(val);
    unsigned long timeElapsed;
    int velocity;
    MaxVal = (val > MaxVal) ? val : MaxVal;
    trigger = val > THRESHOLD && !triggering;
    if (val > THRESHOLD && !triggering) trigger = true;
    if (trigger) {
        triggerTime = millis();
        trigger = false;
        triggering = true;
        triggerBuffer.clear();
    }
    if (triggering) {
        timeElapsed = millis() - triggerTime;
        if (timeElapsed < SAMPLE_TIME) {
            loopCounter++;
            triggerBuffer.addValue(val);
            Serial.println(val);
        }
        else {
            velocity = map(round(triggerBuffer.getMax()), 0, 300, THRESHOLD, 127);
            Serial.printf("Velocity: %d\n", velocity);
            if (comFunctionsSet) sendNote(40, velocity, 1);
            noteEndTime = millis();
            triggerTime = 0;
            triggerBuffer.clear();
            triggering = false;
            resting = true;
        }
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
