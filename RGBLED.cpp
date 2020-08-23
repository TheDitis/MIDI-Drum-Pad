#include <Arduino.h>
#include "RGBLED.h"

RGBLED::RGBLED(int rPin, int gPin, int bPin) {
    /*  Constructor for common cathode RGB LEDs
     *  Pass the PWM-enabled pin number for each color 
     */

    // set the pinNumbers to those provided:
    redPin = rPin;
    greenPin = gPin;
    bluePin = bPin;
    // set all provided pins to outputs
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    // set all color values to 0
    red = 0;
    green = 0;
    blue = 0;
    setColor(red, green, blue);  // set cr
}

RGBLED::RGBLED(int rPin, int gPin, int bPin, int anodePin) {
    /*  Constructor for common anode RGB LEDs
     *  Pass the PWM-enabled pin number for each color 
     *  You can give a gpio pin for anodePin, or pass -1 if you are going to use the boards output power
     */

    // set the pinNumbers to those provided:
    redPin = rPin;
    greenPin = gPin;
    bluePin = bPin;
    // set all provided pins to outputs
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    
    commonAnode = true;  // since a value is given for anodePin
    // if a gpio pin is given for anodePin, set it to an output and set it to high
    if (anodePin >= 0) {
        pinMode(anodePin, OUTPUT);
        digitalWrite(anodePin, HIGH);
    }

    // set all color values to 0
    red = 0;
    green = 0;
    blue = 0;
    setColor(red, green, blue);  // set color
}

void RGBLED::setColor(int r, int g, int b) {
    // set color attributes to given values
    red = r;
    green = g;
    blue = b;

    // if the LED is common anode, invert the values
    if (commonAnode) {
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
    }

    // output color values to corresponding pins
    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
}

void RGBLED::setPins(int rPin, int gPin, int bPin) {
    // set the pinNumbers to those provided:
    redPin = rPin;
    greenPin = gPin;
    bluePin = bPin;
    // set all provided pins to outputs
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    // set all color values to 0
    red = 0;
    green = 0;
    blue = 0;
    setColor(red, green, blue);  // set cr
}

void RGBLED::setPins(int rPin, int gPin, int bPin, int anodePin) {
    // set the pinNumbers to those provided:
    redPin = rPin;
    greenPin = gPin;
    bluePin = bPin;
    // set all provided pins to outputs
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    commonAnode = true;  // since a value is given for anodePin
    // if a gpio pin is given for anodePin, set it to an output and set it to high
    if (anodePin >= 0) {
        pinMode(anodePin, OUTPUT);
        digitalWrite(anodePin, HIGH);
    }

    // set all color values to 0
    red = 0;
    green = 0;
    blue = 0;
    setColor(red, green, blue);  // set color
}

void RGBLED::fadeTo(int r, int g, int b, int msDuration) {
    int startTime = millis();  // time that fade starts
    int timeElapsed = 0;
    // create float versions of each color:
    double rVal = red;
    double gVal = green;
    double bVal = blue;
    // get differences between current and target values for each color:
    double rDiff = r - red;
    double gDiff = g - green;
    double bDiff = b - blue;
    double rInterval, gInterval, bInterval;  // intervals to change by for each color
    // booleans for each color representing whether or not the number of steps between
    // starting and target colors is greater than half the duration in ms
    bool rFast = abs(rDiff * 2) > msDuration;
    bool gFast = abs(gDiff * 2) > msDuration;
    bool bFast = abs(bDiff * 2) > msDuration;

    /*
     *  for each color:
     *     if there is no change to be made:
     *         assign interval to a high number that won't be reached (about 115 days)
     *     otherwise:
     *         if the duration in ms is smaller than twice the color difference:
     *             assign interval to the number of units to increment by per ms
     *         otherwise:
     *             assign interval to the number of ms we want between each value increment for that color
     */

    if (rDiff == 0) rInterval = 9999999999;
    else {
        rInterval = rFast ? (rDiff / msDuration) : (msDuration / rDiff);
    }
    if (gDiff == 0) gInterval = 9999999999;
    else {
        gInterval = gFast ? (gDiff / msDuration) : (msDuration / gDiff);
    }
    if (bDiff == 0) bInterval = 9999999999;
    else {
        bInterval = bFast ? (bDiff / msDuration) : (msDuration / bDiff);
    }

    // for SLOW MODE to make sure each color is only changed once for a given millisecond value
    bool rAlreadyChanged = false;
    bool gAlreadyChanged = false;
    bool bAlreadyChanged = false;
    int prevTime = -1;  // for fast changing colors to change every ms to make sure a change is made only once per ms, rather than once per loop

    // while we are still within the duration given:
    while (timeElapsed < msDuration) {
        timeElapsed = millis() - startTime;  // get amount of time passes since beginning for each iteration

        // FAST MODE Branch
        // If any of the differences in color are greater than half the transition time:
        // This is for any colors that are changing by more than 1 unit per ms
        if (rFast || gFast || bFast) {
            if (timeElapsed != prevTime) { // if this branch hasn't already run once this millisecond:
                // for each color, if it is to change in fast mode, increment it by the corresponding interval
                if (rFast) rVal += rInterval;
                if (gFast) gVal += gInterval;
                if (bFast) bVal += bInterval;
                prevTime = timeElapsed;  // set prevTime to the time this loop last ran
                setColor(rVal, gVal, bVal);
            }
        }

        // SLOW MODE Branch
        // If any of the differences in color are less than half the transition time:
        // This is for any colors that are to change by one unit every N number of milliseconds
        if (!rFast || !gFast || !bFast) {

            // for each color that is changing in slow mode and has a target distance greater than 5:

            // handling slow red fade
            if (!rFast && rDiff > 5) {
                // if the number of ms elapsed is divides evenly by the calculated interval:
                if (timeElapsed % round(rInterval) == 0) {
                    // if this color hasn't changed already this millisecond:
                    if (!rAlreadyChanged) {
                        // if the target difference is negative and the value is positive, reduce by 1
                        if (rDiff < 0  && red > 0) red -= 1;
                        // if the target difference is positive and the value is less than 255, increase by 1
                        else if (rDiff > 0 && red < 255) red += 1;
                        // signify that this color has been changed already this millisecond
                        rAlreadyChanged = true;
                    }
                } else rAlreadyChanged = false;  // if the time elapsed is not evenly divisible by the interval, reset alreadyChanged
            }

            // handling slow green fade
            if (!gFast && gDiff > 5) {
                if (timeElapsed % round(gInterval) == 0) {
                    if (!gAlreadyChanged) {
                        if (gDiff < 0  && green > 0) green -= 1;
                        else if (gDiff > 0 && green < 255) green += 1;
                        gAlreadyChanged = true;
                    }
                } else gAlreadyChanged = false;
            }

            // handling slow blue fade
            if (!bFast && bDiff > 5) {
                if (timeElapsed % round(bInterval) == 0) {
                    if (!bAlreadyChanged) {
                        if (bDiff < 0  && blue > 0) blue -= 1;
                        else if (bDiff > 0 && blue < 255) blue += 1;
                        bAlreadyChanged = true;
                    }
                } else bAlreadyChanged = false;
            }
            setColor(red, green, blue); // set the color to the new calculated values
        }
    }
    setColor(r, g, b);  // make sure color reaches targets exactly, not almost;
}

void RGBLED::setFadeTarget(int r, int g, int b, int msDuration) {
    if (inChange) {
        endFade();
//        bool onHit = rTarget != 0 && gTarget != 255 && bTarget != 255;
//        Serial.printf("FadeNotCompleted. Onhit: %d\n", onHit);
    }
    inChange = true;
    fadeStartTime = millis();
    rStart = red;
    gStart = green;
    bStart = blue;
    rTarget = r;
    gTarget = g;
    bTarget = b;
    fadeDuration = msDuration;
//    Serial.printf("starts red %d green %d blue %d\n", rStart, gStart, bStart);
//    Serial.printf("targets red %d green %d blue %d\n", rTarget, gTarget, bTarget);
}

void RGBLED::update() {

    if (inChange) {
        int rVal, gVal, bVal;
        int timeInFade = millis() - fadeStartTime;
        if (timeInFade > fadeDuration) endFade();
        else {
            red = map(timeInFade, 0, fadeDuration, rStart, rTarget);
            green = map(timeInFade, 0, fadeDuration, gStart, gTarget);
            blue = map(timeInFade, 0, fadeDuration, bStart, bTarget);
//            Serial.printf("red %d green %d blue %d\n\n", red, green, blue);
            setColor(red, green, blue);
        }
    }
}

void RGBLED::endFade() {
    setColor(rTarget, gTarget, bTarget);  // making sure we reached the target exactly;
    inChange = false;
    fadeStartTime = 0;
    fadeDuration = 0;
    rStart = red;
    gStart = green;
    bStart = blue;
    rTarget = red;
    gTarget = green;
    bTarget = blue;
//    rDiff = 0;
//    gDiff = 0;
//    bDiff = 0;
}




















