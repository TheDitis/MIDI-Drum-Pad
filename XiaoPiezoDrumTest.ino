#include <Arduino.h>
#include <MIDI.h>
#include <functional>
//#include <Adafruit_TinyUSB.h>
#include <Adafruit_USBD_MIDI.h>
#include "XiaoPiezoDrum.h"

#include "Display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define DEBUG true  // runs the loop at a max of once per ms so that serial platter is easier to read
int timeInterval = 5;
int prevMs = 0;

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

void sendNoteOn(int note, int velocity, int chan) {
    MIDI.sendNoteOn(note, velocity, chan);
    Serial.println();
};
void sendNoteOff(int note, int velocity, int chan) {
    MIDI.sendNoteOff(note, velocity, chan);
    Serial.println();
};

XiaoPiezoDrum drum(0,  1, 2, 3);




void setup() {
    MIDI.begin(MIDI_CHANNEL_OMNI);
    drum.setNoteOnFunc(sendNoteOn);
    drum.setNoteOffFunc(sendNoteOff);
    drum.setKnobPins(8, 9, 10);
    drum.initDisplay();
}

void loop() {
    if (DEBUG && millis() != prevMs && millis() % timeInterval == 0) {
        prevMs = millis();
        drum.RunCycle();
    }
    else drum.RunCycle();
}
