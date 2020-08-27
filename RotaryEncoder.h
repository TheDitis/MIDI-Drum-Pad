//
// Created by Ryan McKay on 8/23/20.
//

#ifndef XIAOPIEZODRUM_ROTARYENCODER_H
#define XIAOPIEZODRUM_ROTARYENCODER_H

#include <Arduino.h>
#define COLLECTION_TIME 50  // number of ms to count clicks for acceleration (NOT YET USED)

class RotaryEncoder {
public:
    RotaryEncoder() {};
    RotaryEncoder(int pinA, int pinB);
    RotaryEncoder(int pinA, int pinB, int btnPin);
    int checkPosition();  // see if the knob was turned and get the direction if so (left: 0, right: 1, no turn: -1)
    bool wasClicked();  // true if the button is down
    void SerialPlotStates();  // Plot the signals from the knob, along with the direction for debugging

private:
    int pin1, pin2, clkPin;  // input pins for the rotary encoder itself, including the the button pin
    int p1Val, p2Val;  // assigned to the value read from the two encoder pins each time checkPosition is called
    bool isMoving = false; // true if either value is not 1
    int turnInfo = -1;  // no turn: -1, left: 0, right: 1
    bool btnDown = false;  // set to true when the button is pressed
    int direction = -1;  // same as turnInfo, but does not reset to -1 after a turn. Instead, it remains assigned to the last direction the knob was turned indefinitely
    int turnCount = 0; // to determine rotation speed (NOT YET USED)
    int incrementValue = 1; // increased when turned faster (NOT YET USED)
};


#endif //XIAOPIEZODRUM_ROTARYENCODER_H
