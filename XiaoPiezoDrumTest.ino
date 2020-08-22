#include <Arduino.h>
#include <MIDI.h>
#include <functional>
//#include <Adafruit_TinyUSB.h>
#include <Adafruit_USBD_MIDI.h>
#include "XiaoPiezoDrum.h"

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

void sendNoteOn(int note, int velocity, int chan) {
//    Serial.printf("WORKS! %d %d %d\n", note, velocity, chan);
    MIDI.sendNoteOn(note, velocity, chan);
};
void sendNoteOff(int note, int velocity, int chan) {
//    Serial.printf("WORKS! %d %d %d\n", note, velocity, chan);
    MIDI.sendNoteOff(note, velocity, chan);
};

XiaoPiezoDrum drum(9, 2, 3, 4);


void setup() {
    MIDI.begin(MIDI_CHANNEL_OMNI);
    drum.setNoteOnFunc(sendNoteOn);
    drum.setNoteOffFunc(sendNoteOff);
}

void loop() {
    drum.RunCycle();
    delay(1);
}
