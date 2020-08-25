//
// Created by Ryan McKay on 8/23/20.
//

#ifndef XIAOPIEZODRUM_ROTARYENCODER_H
#define XIAOPIEZODRUM_ROTARYENCODER_H

#include <Arduino.h>
#define COLLECTION_TIME 50  // number of ms to count clicks for acceleration

class RotaryEncoder {
public:
    RotaryEncoder() {};
    RotaryEncoder(int pinA, int pinB);
    RotaryEncoder(int pinA, int pinB, int btnPin);
    int checkPosition();
    bool isClicked();
    void SerialPlotStates();

private:
    int pin1, pin2, clkPin;
    int p1Val, p2Val;
    bool isMoving = false; // true if either value is not 1
    int turnInfo = -1;
    bool btnDown = false;
    int direction = 0;
    int turnCount = 0; // to determine rotation speed.
    int incrementVaue = 1; // increased when turned faster
};


#endif //XIAOPIEZODRUM_ROTARYENCODER_H
