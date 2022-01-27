/*
 * Example for TinyButton usage
 */


#include "Arduino.h"
#include <avr/sleep.h>
#include "TinyButton.h"

//arduino uno + keyestudio easy module
#define INSWITCH 3
#define OUTON 13
#define OUTOFF 12

//tiny13 MicroCore
/*
#define INSWITCH 4
#define OUTON 0
#define OUTOFF 1
*/

// Setup a new OneButton on pin PIN_INPUT
// The 2. parameter activeLOW is true, because external wiring sets the button to LOW when pressed.
TinyButton button(INSWITCH, true, true);

byte outState;

void setup() {
  // link the doubleclick function to be called on a doubleclick event.
  button.attachClick(singleClick);
  pinMode(OUTON, OUTPUT);
  pinMode(OUTOFF, OUTPUT);
  outState=LOW;
  
}

void loop() {
  
  // keep watching the push button:
  button.tick();
  
  #if defined(ARDUINO_ARCH_AVR)
  set_sleep_mode(SLEEP_MODE_IDLE); 
  sleep_mode(); //wait for millis update and save some power
  #endif
}

// this function will be called when the button was pressed.
void singleClick()
{
  if ( button.getNumberClicks() == 1 ) { //ignore double and multiclick
    outState = !outState; // reverse the LED
  }
  digitalWrite(OUTON, outState);
  digitalWrite(OUTOFF, !outState);
  
}
