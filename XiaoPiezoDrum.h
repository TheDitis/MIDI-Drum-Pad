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


#define THRESHOLD 15
#define OFF_THRESH 15
#define SAMPLE_TIME 14
#define NOTE_LENGTH 10
#define REST_TIME 20
#define RIMSHOT_THRESH 200

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
    void setNoteOnFunc(const function<void(int, int, int)>& onFunc);
    void setNoteOffFunc(const function<void(int, int, int)>& offFunc);
//    void setNoteComFunctions();

private:
    RGBLED LED;
    unsigned long lastTriggered = millis();
    double MaxVal = 0;  // used for calculating velocity.
    double MaxValRim = 0;
    PiezoSensor sensor;
    int Note = 40;
    int NoteRim = 41;
    int PlayNoteNumber;
    int loopCounter = 0;  // for counting the number of samples gathered during trigger
    bool rimshot = false;
    bool trigger = false;  // to trigger the process of calculating strike velocity
    bool triggering = false;  // for the time period of collecting velocity data before note is sent
    bool triggered = false;  // true once a midi not has been sent and set false when past the note length period
    bool resting = false;  // true once noteoff signal has been sent until REST_TIME ms has passed
    unsigned long triggerStartTime;
    unsigned long triggerSentTime;
    unsigned long noteEndTime;
    RunningAverage triggerBuffer;  // was sampleValues
    bool comFunctionsSet = false;
    function<void(int, int, int)> sendNote;
    function<void(int, int, int)> noteOff;
};

#endif //XIAOPIEZODRUM_XIAOPIEZODRUM_H
