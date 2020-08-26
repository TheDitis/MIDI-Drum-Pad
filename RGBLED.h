#ifndef _RGBLED_H_
#define _RGBLED_H_


class RGBLED {
public:
    RGBLED() {};
    RGBLED(int rPin, int gPin, int bPin);
    RGBLED(int rPin, int gPin, int bPin, int anodePin);
    void setColor(int r, int g, int b);  // change color of led immediately
    void fadeTo(int r, int g, int b, int msDuration);  // a blocking fade method
    void setFadeTarget(int r, int g, int b, int msDuration);  // a non-blocking fade method
    void setPins(int rPin, int gPin, int bPin);  // to set the pins of the led after initialization
    void setPins(int rPin, int gPin, int bPin, int anodePin);  // same as above but for common anode LEDs. Pass -1 for anodePin if not using a gpio pin for the anode
    void update();  // this will check if a fade target has been set and has not yet completed, and then set the color according to how much of the given fade duration has passed

private:
    void endFade();  // runs once a fade target has been reached
    void changeLEDColor();  // outputs the current values of red, green, and blue to the assigned pins
    bool commonAnode = false;  // whether or not the values need to be inverted before setting pins
    int redPin, greenPin, bluePin;  // pin numbers for each color of the LED
    int red, green, blue = 0;  // current color values of the LED
    bool inChange = false;  // whether or not a fade is in progress
    unsigned long fadeStartTime = 0;  // millis() time when setFadeTarget is called
    int fadeDuration = 0;  // duration of the fade
    int rStart, gStart, bStart = 0;  // what the color was before the fade started
    int rTarget, gTarget, bTarget = 0;  // what the color should be when the fade is complete
};

#endif //_RGBLED_H_
