#ifndef XIAOPIEZODRUM_XIAOPIEZODRUM_H
#define XIAOPIEZODRUM_XIAOPIEZODRUM_H
#include <Arduino.h>
//#include <Adafruit_TinyUSB.h>
#include <functional>
#include "Smoothed.h"
#include "RunningAverage.h"
#include "RGBLED.h"
#include "PiezoSensor.h"
#include "Buffer.h"


#define THRESHOLD 5
#define OFF_THRESH 5
#define SAMPLE_TIME 40
#define REST_TIME 30
#define NOTE_LENGTH 20

using namespace std;

//// defining midi helper functions
//void handleNoteOn(int channel, int pitch, int velocity);
//void handleNoteOff(int channel, int pitch, int velocity);



class XiaoPiezoDrum {
public:
    XiaoPiezoDrum();
    XiaoPiezoDrum(int rPin, int gPin, int bPin);
    XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin);
    void RunCycle();
    void setNoteComFunctions(const function<void(int, int, int)>& onFunc);
//    void setNoteComFunctions();

private:
    RGBLED LED;
    unsigned long lastTriggered = millis();
    double MaxVal = 0;
    PiezoSensor sensor;
    int loopCounter = 0;  // for counting the number of samples gathered during trigger
    bool trigger = false;  // to trigger the process of calculating strike velocity
    bool triggering = false;  // for the time period of collecting velocity data before note is sent
    bool triggered = false;  // true once a midi not has been sent and set false when past the note length period
    bool resting = false;  // true once noteoff signal has been sent until REST_TIME ms has passed
    unsigned long triggerTime;
    unsigned long noteEndTime;
    RunningAverage triggerBuffer;  // was sampleValues
    bool comFunctionsSet = false;
    function<void(int, int, int)> sendNote;
    function<void(int, int, int)> noteOff;
};

#endif //XIAOPIEZODRUM_XIAOPIEZODRUM_H
