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

//--- Current color value
AWColor displayedColor = AWColor::white () ;

AWSlider *redSlider;
AWSlider *greenSlider;
AWSlider *blueSlider;
AWSlider *backlightSlider;
AWView *colorView;

//--- button action
void sliderAction (AWView * inSender)
{
  AWSlider * sendingSlider = (AWSlider *) inSender ;
  AWInt pos = sendingSlider->knobPosition ();
  if (sendingSlider == backlightSlider) {
    analogWrite (BACKLIGHT, pos);
  }
  else {
    AWColor newColor(redSlider->knobPosition (), greenSlider->knobPosition (), blueSlider->knobPosition ());
    colorView->setBackColor (newColor) ;
  }
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
  redSlider = new AWSlider (AWPoint (30,30), 400, kVertical, true) ;
  redSlider->setMaxKnobPosition (255);
  redSlider->setKnobPosition (255);
  redSlider->setAction (sliderAction);
  addView (redSlider) ;
  addView (new AWLabel (AWPoint (25, 440), 40, kAWAlignmentCenter, "RED")) ;
  
  greenSlider = new AWSlider (AWPoint (100,30), 400, kVertical, true) ;
  greenSlider->setMaxKnobPosition (255);
  greenSlider->setKnobPosition (255);
  greenSlider->setAction (sliderAction);
  addView (new AWLabel (AWPoint (95, 440), 40, kAWAlignmentCenter, "GREEN")) ;

  blueSlider = new AWSlider (AWPoint (170,30), 400, kVertical, true) ;
  blueSlider->setMaxKnobPosition (255);
  blueSlider->setKnobPosition (255);
  blueSlider->setAction (sliderAction);
  addView (new AWLabel (AWPoint (165, 440), 40, kAWAlignmentCenter, "BLUE")) ;

  backlightSlider = new AWSlider (AWPoint (240,30), 400, kVertical, true) ;
  backlightSlider->setMaxKnobPosition (255);
  backlightSlider->setKnobPosition (200);
  backlightSlider->setAction (sliderAction);
  addView (new AWLabel (AWPoint (235, 440), 40, kAWAlignmentCenter, "LUM")) ;

  addView (greenSlider) ;
  addView (blueSlider) ;
  addView (backlightSlider) ;
  colorView = new AWView (AWRect (350, 30, 420, 420), AWColor::white());
  addView (colorView) ;
}

void loop() {
  AWContext::handleTouchAndDisplay () ;
}
