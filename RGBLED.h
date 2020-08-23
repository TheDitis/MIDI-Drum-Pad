#ifndef _RGBLED_H_
#define _RGBLED_H_


class RGBLED {
public:
    RGBLED() {};
    RGBLED(int rPin, int gPin, int bPin);
    RGBLED(int rPin, int gPin, int bPin, int anodePin);
    void setColor(int r, int g, int b);
    void fadeTo(int r, int g, int b, int msDuration);
    void setFadeTarget(int r, int g, int b, int msDuration);
    void setPins(int rPin, int gPin, int bPin);
    void setPins(int rPin, int gPin, int bPin, int anodePin);
    void update();
    void endFade();

private:
    bool commonAnode = false;  // whether or not the values need to be inverted before setting pins
    int redPin, greenPin, bluePin;  // pin numbers for each color of the LED
    int red, green, blue = 0;  // current color values of the LED
    bool inChange = false;  // whether or not a fade is in progress
    unsigned long fadeStartTime = 0;  // millis() time when setFadeTarget is called
    int fadeDuration = 0;  // duration of the fade
    int rStart, gStart, bStart = 0;  // what the color was before the fade started
    int rTarget, gTarget, bTarget = 0;  // what the color should be when the fade is complete
//    int rDiff, gDiff, bDiff = 0;  // the difference for each color between start and end colors
};

#endif //_RGBLED_H_
