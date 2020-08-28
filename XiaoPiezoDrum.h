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
#include "Display.h"

#define THRESHOLD 20
#define OFF_THRESH 10
#define SAMPLE_TIME 12
#define NOTE_LENGTH 20
#define REST_TIME 20
#define RIMSHOT_THRESH 400
#define MENU_TIMEOUT 5000


using namespace std;


class XiaoPiezoDrum {
public:
    XiaoPiezoDrum();
    XiaoPiezoDrum(int rPin, int gPin, int bPin);
    XiaoPiezoDrum(int piezoPin, int rPin, int gPin, int bPin);
    void RunCycle();  // this is the main process that runs each loop
    void setNoteOnFunc(const function<void(int, int, int)>& onFunc);  // to pass the desired midi noteOn handler to the class
    void setNoteOffFunc(const function<void(int, int, int)>& offFunc);  // to pass the desired midi noteOff handler to the class
    void setKnobPins(int pin1, int pin2, int clkPin);  // to set up the navigation knob
    void initDisplay();

private:
    RGBLED LED;  // Indicator LED
    Display display;
    PiezoSensor sensor;  // The drum sensor itself, which is just a small piezo sensor connect between the provided GPIO pin and gnd, with a resistor between
    RunningAverage triggerBuffer;  // the buffer that we use to collect velocity data once a hit is detected
    RotaryEncoder knob;  // the knob used to change notes and navigate menus (menus not yet implemented)
    function<void(int, int, int)> sendNoteHandler;  // blank noteOn handler to be assigned to your midi protocol handler using setNoteOnFunc method
    function<void(int, int, int)> noteOffHandler;  // blank noteOff handler to be assigned to your midi protocol handler using setNoteOffFunc method
    double SensorReading = 0;  // stores the most recent sensor reading
    double MaxVal = 0;  // used for calculating velocity of plain hits
    double MaxValRim = 0;  // used for calculating velocity of rimshots
    int Note = 40;  // note sent when the peak sensor value collected during triggering stage is below RIMSHOT_THRESH
    int NoteRim = 41;  // note sent when the peak sensor value collected during triggering stage is above RIMSHOT_THRESH
    int PlayNoteNumber;  // alternates between the values of Note and NoteRim based on how hard the hit was
    bool changeRimshotNote = false;  // whether turning the knob should change the secondary note instead of the primary
    int loopCounter = 0;  // for counting the number of samples gathered during strikeDetected
    bool rimshot = false;  // whether or not the output note should be the primary or the rimshot note
    bool strikeDetected = false;  // calls TriggerHit when the conditions are all right
    bool triggering = false;  // for the time period of collecting velocity data before note is sent
    bool triggered = false;  // true once a midi not has been sent and set false when past the note length period
    bool resting = false;  // true once noteOff signal has been sent until REST_TIME ms has passed
    unsigned long triggerStartTime; // the time sensor data collection started
    unsigned long triggerSentTime;  // time the noteOn signal was sent, used to calculate when to send the noteOff signal
    unsigned long noteEndTime;  // time the noteOff signal was sent, at which point the resting time countdown begins
    bool comFunctionsSet = false;  // whether or not the two methods below have been set using the setNoteOnFunc and setNoteOffFunc methods
    void TriggerHit();  // method that starts the process of collecting data and sending a note
    void CalculateAndSendNote();  // method that handles the collection of velocity data and sending the noteOn signal
    void NoteOffCountdown();  // method that starts being called each loop once note is sent, and counts down NOTE_LENGTH ms, before sending noteOff signal
    void DetermineRestingState();  // method starts being called each loop after noteOff signall is sent, until REST_TIME ms has passed, at which point it sets parameters such that a new note can be sent
    void SerialPlotStages();  // used for debugging using the arduino serial plotter. It plots the sensor readings, along with signifiers of when each of the stages (triggering, triggered, and resting)
    int knobState = -1;  // assigned every loop with the knob's checkPosition method. no turn: -1, left: 0, right: 1
    int btnClicked = false;  // assigned with the knob's wasClicked method every loop;

    bool onHomeScreen = true; // true if you are not in a menu
    bool onSettingsScreen = !onHomeScreen; // true if you are in the menu. Mutually exclusive to onHomeScreen
    void toggleSettingsScreen();
    bool menuScrolling = true;  // true when knob turns should move the menu selection cursor rather than change values
    bool changeMenuValue = !menuScrolling;  // true when you should be changing values in the menu. Mutually exclusive to menuScrolling
    void toggleMenuSelect() {menuScrolling = !menuScrolling; changeMenuValue = !changeMenuValue;}  // method to switch between menu item selection and value editing
    void handleKnobTurn();  // determines what happens when the knob is turned
    void handleButtonClick();  // run any time the rotary encoder knob is pressed.
    int homeScreenSelection = 0;  // this will determine which item is selected when we are in the home menu
    int settingsSelection = 0;  // this will determine which item is selected when we are in the settings menu
};

#endif //XIAOPIEZODRUM_XIAOPIEZODRUM_H
