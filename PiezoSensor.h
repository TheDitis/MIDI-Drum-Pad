//
// Created by Ryan McKay on 8/20/20.
//

#ifndef XIAOPIEZODRUM_PIEZOSENSOR_H
#define XIAOPIEZODRUM_PIEZOSENSOR_H
#include "Smoothed.h"
#include "Buffer.h"


#define SMOOTH_AMT 100
#define OFFSET -6

class PiezoSensor {
public:
    PiezoSensor();
    explicit PiezoSensor(int sensorPin);
    PiezoSensor(int sensorPin, int smoothAmount);
    double read();

private:
    int PinNumber = -1;
    Smoothed<double> buffer;
//    SmoothedValues.begin(SMOOTHED_AVERAGE, smoothAmount);
//    RunningAverage sampleValues;

};


#endif //XIAOPIEZODRUM_PIEZOSENSOR_H
