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
    PinNumber = sensorPin;
    pinMode(PinNumber, OUTPUT);
}

PiezoSensor::PiezoSensor(int sensorPin, int smoothAmount) {
    PinNumber = sensorPin;
    pinMode(PinNumber, OUTPUT);
//    buffer = Buffer(100, smoothAmount);
    buffer.begin(SMOOTHED_EXPONENTIAL, smoothAmount);
}

double PiezoSensor::read() {
    double val = analogRead(PinNumber);
    buffer.add(val + OFFSET);
    return buffer.get();
}
