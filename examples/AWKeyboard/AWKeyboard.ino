/*===========================================================================
 * AWKeyboard shows how to use the Keyboard of AW
 */

#include <ArduinoWidgets.h>
#include <UTFT.h>

static const byte RS    = 23 ;
static const byte WR    = 22 ;
static const byte CS    = 15 ;
static const byte RESET = 33 ;

//--- Warning D33 of Teensy 3.1: https://forum.pjrc.com/threads/24823-Teensy-3-1-Tying-Pin-33-(pta4)-low-freezes-teensy

//--- LCD type (see /Applications/Arduino.app/Contents/Java/hardware/teensy/avr/libraries/UTFT/UTFT.h)
//    Do not change 'myGLCD' name; it is declared as extern in AWContext.cpp
UTFT myGLCD (SSD1963_800ALT, RS, WR, CS, RESET) ;

static const byte T_CLK  = 11 ;
static const byte T_CS   = 12 ;
static const byte T_DIN  = 25 ;
static const byte T_DOUT = 24 ;
static const byte T_IRQ  = 28 ;

static const byte BACKLIGHT = 9 ;

//--- Do not change 'myTouch' name; it is declared as extern in AWContext.cpp
AWTouch myTouch (T_CLK, T_CS, T_DIN, T_DOUT, T_IRQ) ;

AWLabel *keyboardResult ;

// Action to validate a keyboard entry
void keyboardValidateEntryAction(const String &inText, const int tag)
{
  keyboardResult->setTitle (inText) ;
}

// Action to display the keyboard
void displayKeyboardAction(AWView *sender)
{
  launchKeyboard (keyboardResult->title (), 40, keyboardValidateEntryAction, 0) ;
}

void setup() {
  // set up the backlight
  analogWrite (BACKLIGHT, 200);

  // put your setup code here, to run once:
  AWContext::begin (kOrientationLandscape,
                    800,      // Screen width
                    480,      // Screen height
                    true,     // true : X is flipped
                    false) ;  // false : Y is not flipped

  keyboardResult = new AWLabel (AWPoint (0, 0), 500, kAWAlignmentCenter, "") ;
  addCenteredView (keyboardResult) ;
  
  // Push button to display the Keyboard
  AWPushButton *launchKeyboardButton = new AWPushButton (AWRect (20, 20, 150, 40), "Show keyboard");
  launchKeyboardButton->setAction (displayKeyboardAction) ;
  addView (launchKeyboardButton) ;
}

void loop() {
  AWContext::handleTouchAndDisplay () ;
}
