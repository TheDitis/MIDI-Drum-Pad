//
// Created by Ryan McKay on 8/23/20.
//

#include <Arduino.h>
#include "RotaryEncoder.h"


RotaryEncoder::RotaryEncoder(int pinA, int pinB, int btnPin) {
    pin1 = pinA;
    pin2 = pinB;
    clkPin = btnPin;
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    pinMode(clkPin, INPUT);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(clkPin, HIGH);
}

int RotaryEncoder::checkPosition() {
    // if knob is being turned:
    bool wasTurned = false;
    bool sameDirection;
    bool moving;
    // read encoder pins:
    p1Val = digitalRead(pin1);
    p2Val = digitalRead(pin2);
    moving = !p1Val || !p2Val;  // if either value is not high, indicate that it's in motion.
    // if the knob was not in motion last loop but is this loop:
    if (moving && !isMoving) {
        // getting the direction of turning based on which pin drops voltage first
        // the pin which showed value change first, indicating direction. 0 for left, 1 for right:
        turnInfo = p1Val < p2Val ? 0 : 1;
        sameDirection = direction == turnInfo;  // if this turn is in the same direction as the last
        direction = turnInfo;  // set direction to that determined
        wasTurned = true;
    }
    else turnInfo = -1;
    isMoving = moving;
    return turnInfo;
}

bool RotaryEncoder::isClicked() {
    btnDown = digitalRead(clkPin);
    return btnDown;
}

void RotaryEncoder::SerialPlotStates() {
    Serial.print("direction:");
    Serial.print(direction);
    Serial.print("\t");
    Serial.print("one:");
    Serial.print(p1Val);
    Serial.print("\t");
    Serial.print("two:");
    Serial.print(p2Val);
    Serial.print("\t");
//    Serial.print("clk: ");
//    Serial.print(btnDown);
//    Serial.print("\t");
    Serial.println();
    Serial.println();
}