// -----
// TinyButton.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button. This class is implemented for use with the
// Arduino environment. Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// 02.10.2010 created by Matthias Hertel
// 21.04.2011 transformed into a library
// 01.12.2011 include file changed to work with the Arduino 1.0 environment
// 23.03.2014 Enhanced long press functionalities by adding longPressStart and
// longPressStop callbacks
// 21.09.2015 A simple way for debounce detection added.
// 14.05.2017 Debouncing improvements.
// 25.06.2018 Optional third parameter for deactivating pullup.
// 26.09.2018 Anatoli Arkhipenko: Included solution to use library with other
// sources of input.
// 26.09.2018 Initialization moved into class declaration.
// 26.09.2018 Jay M Ericsson: compiler warnings removed.
// 29.01.2020 improvements from ShaggyDog18
// -----

#ifndef TinyButton_h
#define TinyButton_h

#include "Arduino.h"

//#define MULTIEND

#define DEBOUNCEMS   10
#define CLICKMS     200
#define PRESSMS     800

// ----- Callback function types -----

extern "C" {
typedef void (*callbackFunction)(void);
}


class TinyButton
{
public:
  // ----- Constructor -----
  TinyButton();

  /**
   * Initialize the TinyButton library.
   * @param pin The pin to be used for input from a momentary button.
   * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
   * @param pullupActive Activate the internal pullup when available. Default is true.
   */
  TinyButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);

  // ----- Set runtime parameters -----

  /**
   * Attach an event to be called when a single click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachClick(callbackFunction newFunction);

  /**
   * Attach an event to fire when the button is pressed and held down.
   * @param newFunction
   */
  void attachLongPressStart(callbackFunction newFunction);

  /**
   * Attach an event to fire as soon as the button is released after a long press.
   * @param newFunction
   */
  void attachLongPressStop(callbackFunction newFunction);

  /**
   * Attach an event to fire periodically while the button is held down.
   * @param newFunction
   */
  void attachDuringLongPress(callbackFunction newFunction);

  
  /*
   * return number of clicks in any case: single or multiple clicks
   */  
  byte getNumberClicks(void);
  
  /*
   * return press duration in ms
   */
  unsigned long getPressDuration(void);
  
  
  // ----- State machine functions -----

  /**
   * @brief Call this function every some milliseconds for checking the input
   * level at the initialized digital pin.
   */
  void tick(void);


  /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
  void tick(bool level);


  /**
   * @return true if we are currently handling button press flow
   * (This allows power sensitive applications to know when it is safe to power down the main CPU)
   */
  bool isIdle() const { return _state == OCS_INIT; }

  /**
   * @return true when a long press is detected
   */
  bool isLongPressed() const { return _state == OCS_PRESS; };


private:
  uint8_t _pin;                         // hardware pin number.
  byte _buttonPressed;
  byte _nClicks;
  
  // These variables will hold functions acting as event source.
  callbackFunction _clickFunc = NULL;

  callbackFunction _longPressStartFunc = NULL;

  callbackFunction _longPressStopFunc = NULL;

  callbackFunction _duringLongPressFunc = NULL;

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the
  // tick function is called.

  // define FiniteStateMachine
  enum stateMachine_t : byte {
    OCS_INIT = 0,
    OCS_DOWN = 1,
    OCS_UP = 2,
    OCS_COUNT = 3,
    OCS_PRESS = 6,
    OCS_PRESSEND = 7,
    UNKNOWN = 99
  };

  /**
   *  Advance to a new state and save the last one to come back in cas of bouncing detection.
   */
  void _newState(stateMachine_t nextState);
  
  stateMachine_t _state = OCS_INIT;
  stateMachine_t _lastState = OCS_INIT; // used for debouncing

  unsigned long _startTime; // start of current input change to checking debouncing

    
};

#endif
