//
// Created by Ryan McKay on 8/20/20.
//

#ifndef XIAOPIEZODRUM_PIEZOSENSOR_H
#define XIAOPIEZODRUM_PIEZOSENSOR_H
#include "Smoothed.h"


#define SMOOTH_AMT 100  // NOT CURRENTLY USED
#define OFFSET (-14)  // to offset the resting level of the sensor so that it sits at 0

class PiezoSensor {
public:
    PiezoSensor();
    explicit PiezoSensor(int sensorPin);
    PiezoSensor(int sensorPin, int smoothAmount);
    double read(); // gets smoothed value

private:
    unsigned int PinNumber;  // pin number of the piezo sensor on the board
    Smoothed<double> buffer;  // declare smoothing buffer
};


#endif //XIAOPIEZODRUM_PIEZOSENSOR_H
