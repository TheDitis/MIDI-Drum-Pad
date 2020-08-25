#ifndef XIAOPIEZODRUM_XIAOPIEZODRUM_H
#define XIAOPIEZODRUM_XIAOPIEZODRUM_H
#include <Arduino.h>
//#include <Adafruit_TinyUSB.h>
#include <functional>
#include "RotaryEncoder.h"
#include "Smoothed.h"
#include "RunningAverage.h"
#include "RGBLED.h"
#include "PiezoSensor.h"

#define THRESHOLD 20
#define OFF_THRESH 10
#define SAMPLE_TIME 12
#define NOTE_LENGTH 20
#define REST_TIME 20
#define RIMSHOT_THRESH 400
#define MENU_TIMEOUT 5000

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
    void setKnobPins(int pin1, int pin2, int clkPin);
//    void setNoteComFunctions();

private:
    RGBLED LED;
    bool homeScreen = true; // true if you are not in a menu
//    unsigned long lastTriggered = millis();
    double MaxVal = 0;  // used for calculating velocity.
    double MaxValRim = 0;
    PiezoSensor sensor;
    double SensorReading = 0;  // stores the most recent sensor reading
    int Note = 40;  // note sent when the peak sensor value collected during triggering stage is below RIMSHOT_THRESH
    int NoteRim = 41;  // note sent when the peak sensor value collected during triggering stage is above RIMSHOT_THRESH
    int PlayNoteNumber;  //
    bool changeRimshotNote = false;  // whether turning the knob should change the secondary note instead of the primary
    int loopCounter = 0;  // for counting the number of samples gathered during strikeDetected
    bool rimshot = false;  // whether or not the output note should be the primary or the rimshot note
    bool strikeDetected = false;  // calls TriggerHit when the conditions are all right
    bool triggering = false;  // for the time period of collecting velocity data before note is sent
    bool triggered = false;  // true once a midi not has been sent and set false when past the note length period
    bool resting = false;  // true once noteoff signal has been sent until REST_TIME ms has passed
    unsigned long triggerStartTime; // the time sensor data collection started
    unsigned long triggerSentTime;  // time the noteOn signal was sent, used to calculate when to send the noteOff signal
    unsigned long noteEndTime;  // time the noteOff signal was sent, at which point the resting time countdown begins
    RunningAverage triggerBuffer;  // the buffer that we use to collect velocity data once a hit is detected
    RotaryEncoder knob;  // the knob used to change notes and navigate menus (menus not yet implemented)
    bool comFunctionsSet = false;  // whether or not the two methods below have been set using the setNoteOnFunc and setNoteOffFunc methods
    function<void(int, int, int)> sendNoteHandler;  // blank noteOn handler to be assigned to your midi protocol handler using setNoteOnFunc method
    function<void(int, int, int)> noteOffHandler;  // blank noteOff handler to be assigned to your midi protocol handler using setNoteOffFunc method
    void TriggerHit();  // method that starts the process of collecting data and sending a note
    void CalculateAndSendNote();  // method that handles the collection of velocity data and sending the noteOn signal
    void NoteOffCountdown();  // method that starts being called each loop once note is sent, and counts down NOTE_LENGTH ms, before sending noteOff signal
    void DetermineRestingState();  // method starts being called each loop after noteOff signall is sent, until REST_TIME ms has passed, at which point it sets parameters such that a new note can be sent
    void SerialPlotStages();  // used for debugging using the arduino serial plotter. It plots the sensor readings, along with signifiers of when each of the stages (triggereing, triggered, and resting)
};

#endif //XIAOPIEZODRUM_XIAOPIEZODRUM_H
