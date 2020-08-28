//
// Created by Ryan McKay on 8/20/20.
//
#include <Arduino.h>
#include "Smoothed.h"
#include "PiezoSensor.h"
#include "XiaoPiezoDrum.h"


PiezoSensor::PiezoSensor() {
}

PiezoSensor::PiezoSensor(int sensorPin) {
    PinNumber = sensorPin;  // assign pin number
    pinMode(PinNumber, INPUT_PULLUP);  // set pin to input
}

PiezoSensor::PiezoSensor(int sensorPin, int smoothAmount) {
    PinNumber = sensorPin;  // assign pin number
    pinMode(PinNumber, INPUT_PULLUP);  // set pin to input
    buffer.begin(SMOOTHED_EXPONENTIAL, smoothAmount);  // set the smoothing parameters for the buffer
}

double PiezoSensor::read() {
    double val = analogRead(PinNumber);  // get the latest value from the sensor
    buffer.add(val + OFFSET);  // add it to the buffer, correcting for the offset
    return buffer.get();  // return the smoothed value
}
