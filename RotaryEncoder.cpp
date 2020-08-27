//
// Created by Ryan McKay on 8/23/20.
//

#include <Arduino.h>
#include "RotaryEncoder.h"


RotaryEncoder::RotaryEncoder(int pinA, int pinB, int btnPin) {
    // assign pins:
    pin1 = pinA;
    pin2 = pinB;
    clkPin = btnPin;
    // set all pins to inputs:
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    pinMode(clkPin, INPUT);
    // write to high to remove signal noise:
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(clkPin, HIGH);
}

int RotaryEncoder::checkPosition() {
    bool sameDirection;  // NOT YET USED
    bool moving;  // temporary value determining whether or not the knob is being turned this loop.
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
    }
    // Otherwise, (if either the knob is not in motion, or it is and the signal has already been registered):
    else turnInfo = -1;  // reset turnInfo to -1, indicating no turn
    isMoving = moving;  // assign isMoving to moving so that comparison can be made next loop
    return turnInfo;  // return turnInfo (no turn: -1, left: 0, right: 1)
}

bool RotaryEncoder::wasClicked() {
    bool buttonDown = digitalRead(clkPin);  // read the pin connected to the button of the encoder
    buttonDown = !buttonDown;  // invert buttonDown, since its a normally closed switch
    bool wasClicked = false;  // set wasClicked to false by default
    if (!btnDown && buttonDown) wasClicked = true;  // if the button was not down last loop but was this one, set wasClicked to true
    btnDown = buttonDown;  // set btnDown attribute to the temporary one so that the comparison can be made next loop
    return wasClicked;  // return true if there was a click event
}

void RotaryEncoder::SerialPlotStates() {
    // print all important values with their labels, output in proper format for arduino serial plotter
    Serial.print("direction:");
    Serial.print(direction);
    Serial.print("\t");
    Serial.print("one:");
    Serial.print(p1Val);
    Serial.print("\t");
    Serial.print("two:");
    Serial.print(p2Val);
    Serial.print("\t");
    Serial.print("clk: ");
    Serial.print(btnDown);
    Serial.print("\t");
    Serial.println();
    Serial.println();
}