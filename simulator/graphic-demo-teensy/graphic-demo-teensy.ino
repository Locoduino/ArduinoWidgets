#include "graphic-setup-loop.h"
#include "touch-callbacks.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __MK66FX1M0__
  #error "Ce sketch doit être compilé pour la Teensy 3.6"
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    CONFIGURE UTFT
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include <UTFT.h>

static const byte RS  = 23 ;
static const byte WR  = 22 ;
static const byte CS  = 15 ;
static const byte RST = 33 ;
static const byte BACKLIGHT = 9 ;

//--- Warning for D33 on Teensy 3.1 : https://forum.pjrc.com/threads/24823-Teensy-3-1-Tying-Pin-33-(pta4)-low-freezes-teensy

//--- LCD type (see /Applications/Arduino.app/Contents/Java/hardware/teensy/avr/libraries/UTFT/UTFT.h)
//    Do not change 'myGLCD' name; it is declared as extern in PMContext.cpp
UTFT myGLCD (SSD1963_800ALT, RS, WR, CS, RST) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    CONFIGURE TOUCH SCREEN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "AWTouch.h"

static const byte T_CLK  = 11 ;
static const byte T_CS   = 12 ;
static const byte T_DIN  = 25 ;
static const byte T_DOUT = 24 ;
static const byte T_IRQ  = 28 ;

//--- Do not change 'myTouch' name; it is declared as extern in PMContext.cpp
AWTouch myTouch (T_CLK, T_CS, T_DIN, T_DOUT, T_IRQ) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    SET UP
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setup (void) {
//--- Backlight
  pinMode (BACKLIGHT, OUTPUT) ;

//--- Builtin led will blink
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;

//--- LCD setup
  myGLCD.InitLCD (LANDSCAPE) ; // or PORTRAIT
  myGLCD.clrScr () ;
  digitalWrite (BACKLIGHT, HIGH) ; // Backlight on

//--- Widget setup (see graphic-setup-loop.cpp)
  graphicSetup () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    LOOP
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const uint32_t BACKLIGHT_OFF_DELAY = 60 * 1000 ; // In ms
static uint32_t gBacklightOffDate = BACKLIGHT_OFF_DELAY ;
static uint32_t gBuiltinLedBlinkDate = 0 ;
static bool gDisplayOn = true ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void loop (void) {
//-------------------- Handle touch event, screen display (see graphic-setup-loop.cpp)
  graphicLoop () ;
//-------------------- Blink builtin led
  if (gBuiltinLedBlinkDate < millis ()) {
    gBuiltinLedBlinkDate += 1000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
//-------------------- Backlight off ?
  if (gBacklightOffDate < millis ()) {
    digitalWrite (BACKLIGHT, LOW) ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// This callback function is called when a touch is detected.
// It is used for switching backlight on

bool callBackTouchOn (void) {
  const bool handleTouch = gDisplayOn ;
//--- Backlight on
  gBacklightOffDate = millis () + BACKLIGHT_OFF_DELAY ;
  digitalWrite (BACKLIGHT, HIGH) ;
  gDisplayOn = true ;
//---
  return handleTouch ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// This callback function is called when no touch is detected.

void callBackNoTouch (void) {
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

