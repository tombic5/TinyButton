/**
 * @file TinyButton.cpp
 *
 * @brief Library for detecting button clicks, doubleclicks and long press
 * pattern on a single button.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license. See
 * http://www.mathertel.de/License.aspx
 *
 * More information on: https://www.mathertel.de/Arduino/TinyButtonLibrary.aspx
 *
 * Changelog: see TinyButton.h
 */

#include "TinyButton.h"

// ----- Initialization and Default Values -----

/**
 * @brief Construct a new TinyButton object but not (yet) initialize the IO pin.
 */
TinyButton::TinyButton()
{
  _pin = -1;
  // further initialization has moved to TinyButton.h
}

/**
 * Initialize the TinyButton library.
 * @param pin The pin to be used for input from a momentary button.
 * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
 * @param pullupActive Activate the internal pullup when available. Default is true.
 */
TinyButton::TinyButton(const int pin, const boolean activeLow, const bool pullupActive)
{
  // TinyButton();
  _pin = pin;

  if (activeLow) {
    // the button connects the input pin to GND when pressed.
    _buttonPressed = LOW;

  } else {
    // the button connects the input pin to VCC when pressed.
    _buttonPressed = HIGH;
  } // if

  if (pullupActive) {
    // use the given pin as input and activate internal PULLUP resistor.
    pinMode(pin, INPUT_PULLUP);
  } else {
    // use the given pin as input
    pinMode(pin, INPUT);
  } // if
} // TinyButton


// save function for click event
void TinyButton::attachClick(callbackFunction newFunction)
{
  _clickFunc = newFunction;
} // attachClick


// save function for multiClick event
void TinyButton::attachMultiClick(callbackFunction newFunction)
{
  _multiClickFunc = newFunction;
} // attachMultiClick


// save function for longPressStart event
void TinyButton::attachLongPressStart(callbackFunction newFunction)
{
  _longPressStartFunc = newFunction;
} // attachLongPressStart


// save function for longPressStop event
void TinyButton::attachLongPressStop(callbackFunction newFunction)
{
  _longPressStopFunc = newFunction;
} // attachLongPressStop


// save function for during longPress event
void TinyButton::attachDuringLongPress(callbackFunction newFunction)
{
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress


// ShaggyDog ---- return number of clicks in any case: single or multiple clicks
byte TinyButton::getNumberClicks(void)
{
  return _nClicks;
}

// 
unsigned long TinyButton::getPressDuration(void)
{
  return millis() - _startTime;
}

/**
 * @brief Check input of the configured pin and then advance the finite state
 * machine (FSM).
 */
void TinyButton::tick(void)
{
  if (_pin >= 0) {
    tick(digitalRead(_pin) == _buttonPressed);
  }
}


/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void TinyButton::_newState(stateMachine_t nextState)
{
  _lastState = _state;
  _state = nextState;
} // _newState()

/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void TinyButton::tick(bool isPressed)
{
  unsigned long now = millis(); // current (relative) time in msecs.
  unsigned long waitTime = (now - _startTime);

  // Implementation of the state machine
  switch (_state) {
  case TinyButton::OCS_INIT:
    // waiting for level to become active.
    if (isPressed) {
      _newState(TinyButton::OCS_DOWN);
      _startTime = now; // remember starting time
      _nClicks = 0;
    } // if
    break;

  case TinyButton::OCS_DOWN:
    // waiting for level to become inactive.

    if ((!isPressed) && (waitTime < DEBOUNCEMS)) {
      // button was released too quickly so I assume some bouncing.
      _newState(_lastState);

    } else if (!isPressed) {
      _newState(TinyButton::OCS_UP);
      _startTime = now; // remember starting time

    } else if ((isPressed) && (waitTime > PRESSMS)) {
      if (_longPressStartFunc) _longPressStartFunc();
      _newState(TinyButton::OCS_PRESS);
    } // if
    break;

  case TinyButton::OCS_UP:
    // level is inactive

    if ((isPressed) && (waitTime < DEBOUNCEMS)) {
      // button was pressed to quickly so I assume some bouncing.
      _newState(_lastState); // go back

    } else if (waitTime >= DEBOUNCEMS) {
      // count as a short button down
      _newState(TinyButton::OCS_COUNT);
      _nClicks++;
      if (_nClicks == 1 || (!_multiClickFunc) ) {
        // 1st click or no multi click is registered.
        if (_clickFunc) _clickFunc();

      } else {
        // this was a multi click sequence.
        _multiClickFunc();
      } // if

    } // if
    break;

  case TinyButton::OCS_COUNT:
    // dobounce time is over, count clicks

    if (isPressed) {
      // button is down again
      _newState(TinyButton::OCS_DOWN);
      _startTime = now; // remember starting time

    } else if ((waitTime > CLICKMS) ) {
      _newState(TinyButton::OCS_INIT);
      //_nClicks=0; save few bytes for TinyCore/MicroCore
        
    } // if
    break;

  case TinyButton::OCS_PRESS:
    // waiting for menu pin being release after long press.

    if (!isPressed) {
      _newState(TinyButton::OCS_PRESSEND);
      _startTime = now;

    } else {
      // still the button is pressed
      if (_duringLongPressFunc) _duringLongPressFunc();
    } // if
    break;

  case TinyButton::OCS_PRESSEND:
    // button was released.

    if ((isPressed) && (waitTime < DEBOUNCEMS)) {
      // button was released to quickly so I assume some bouncing.
      _newState(_lastState); // go back

    } else if (waitTime >= DEBOUNCEMS) {
      if (_longPressStopFunc) _longPressStopFunc();
      _newState(TinyButton::OCS_INIT);
    }
    break;

  default:
    // unknown state detected -> reset state machine
    _newState(TinyButton::OCS_INIT);
    break;
  } // if

} // TinyButton.tick()


// end.
