#ifndef _RGBLED_H_
#define _RGBLED_H_


class RGBLED {
public:
    RGBLED() {};
    RGBLED(int rPin, int gPin, int bPin);
    RGBLED(int rPin, int gPin, int bPin, int anodePin);
    void setColor(int r, int g, int b);
    void fadeTo(int r, int g, int b, int msDuration);
    void fadeToNonBlocking(int r, int g, int b, int msDuration);
    void setPins(int rPin, int gPin, int bPin);
    void setPins(int rPin, int gPin, int bPin, int anodePin);
    void update();

private:
    bool commonAnode = false;
    int redPin, greenPin, bluePin;
    int red = 0;
    int green = 0;
    int blue = 0;
    bool inChange = false;
    int fadeStartTime = 0;
    int fadeDuration = 0;
    int rTarget = 0;
    int gTarget = 0;
    int bTarget = 0;
};

#endif //_RGBLED_H_
