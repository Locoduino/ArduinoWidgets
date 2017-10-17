#include <ArduinoWidgets.h>
#include <UTFT.h>

class ClippingView : public AWView
{
  public : ClippingView (const AWRect & inViewFrame);
  public : virtual void drawInRegion (const AWRegion & inRegion) const;
};

ClippingView::ClippingView (const AWRect & inViewFrame) :
AWView(inViewFrame, AWColor::darkGray ())
{
}

void ClippingView::drawInRegion (const AWRegion & inRegion) const
{
  AWRegion drawingRegion = inRegion ;
  AWRect viewFrame = absoluteFrame () ;
  AWRect clipRectangle = viewFrame ;
  clipRectangle.size.width /= 4 ;
  clipRectangle.size.height /= 4 ;
  drawingRegion -= clipRectangle ;
  clipRectangle.translateBy (viewFrame.size.width - clipRectangle.size.width, 0);
  drawingRegion -= clipRectangle ;
  clipRectangle.translateBy (0, viewFrame.size.height - clipRectangle.size.height);
  drawingRegion -= clipRectangle ;
  clipRectangle.translateBy (clipRectangle.size.width - viewFrame.size.width, 0);
  drawingRegion -= clipRectangle ;
  AWContext::setColor (AWColor::red ());
  viewFrame.fillOvalInRegion (drawingRegion) ;
}

ClippingView * crossView ;

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

void setup() {
  // set up the backlight
  analogWrite (BACKLIGHT, 200) ;

  // put your setup code here, to run once:
  AWContext::begin (kOrientationLandscape,
                    800,      // Screen width
                    480,      // Screen height
                    true,     // true : X is flipped
                    false) ;  // false : Y is not flipped

  // create the target view centered on screen
  crossView = new ClippingView(AWRect (0, 0, 300, 300)) ;
  addCenteredView (crossView) ;
}

void loop() {
  AWContext::handleTouchAndDisplay () ;
}
