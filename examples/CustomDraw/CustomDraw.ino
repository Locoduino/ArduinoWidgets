#include <ArduinoWidgets.h>
#include <UTFT.h>

class CustomView : public AWView
{
  public : CustomView (const AWRect & inViewFrame);
  public : virtual void drawInRegion (const AWRegion & inRegion) const;
  public : void rotateColors ();

  private : AWColor centerColor;
  private : AWColor mediumColor;
  private : AWColor externalColor;
};

CustomView::CustomView (const AWRect & inViewFrame) :
AWView(inViewFrame, AWColor::darkGray ()),
centerColor (AWColor::red ()),
mediumColor (AWColor::white ()),
externalColor (AWColor::blue ())
{
}

void drawLines (const AWRegion & inRegion, const AWPoint & inOrigin)
{
  AWPoint p = inOrigin ;
  p.translateBy ( 10, 0 ) ;
  inOrigin.strokeLineInRegion (p, inRegion) ;
  p.translateBy ( -4, 8 ) ;
  inOrigin.strokeLineInRegion (p, inRegion) ;
}

void CustomView::drawInRegion (const AWRegion & inRegion) const
{
  AWRect viewFrame = absoluteFrame () ;
  AWContext::setColor (externalColor) ;
  viewFrame.fillOvalInRegion (inRegion) ;
  AWInt insetX = viewFrame.size.width / 6 ;
  AWInt insetY = viewFrame.size.height / 6 ;
  AWContext::setColor (mediumColor) ;
  viewFrame.inset (insetX, insetY) ;
  viewFrame.fillOvalInRegion (inRegion) ;  
  AWContext::setColor (centerColor) ;
  viewFrame.inset (insetX, insetY) ;
  viewFrame.fillOvalInRegion (inRegion) ; 

  viewFrame = absoluteFrame () ;
  AWPoint center ;
  center.x = viewFrame.origin.x + viewFrame.size.width / 2 ;
  center.y = viewFrame.origin.y + viewFrame.size.height / 2 ;
  AWPoint tail ;
  tail.x = center.x + viewFrame.size.width / 2 ;
  tail.y = center.y + viewFrame.size.height / 4 ;
  AWContext::setColor (AWColor::black ()) ;
  center.strokeLineInRegion (tail, inRegion) ;
  drawLines (inRegion, center) ;
  center.translateBy (viewFrame.size.width / 2 - 20, viewFrame.size.width / 4 - 10) ; 
  for (int i = 0 ; i < 4 ; i++) {
    drawLines (inRegion, center) ;
    center.translateBy (4, 2) ;
  }
}

void CustomView::rotateColors ()
{
  AWColor centerSave = centerColor ;
  centerColor = mediumColor ;
  mediumColor = externalColor ;
  externalColor = centerSave ;
  setNeedsDisplay () ;
}

CustomView * targetView ;

void rotate (AWView * sender)
{
  targetView->rotateColors () ;
}

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
  targetView = new CustomView(AWRect (0, 0, 300, 300)) ;
  addCenteredView (targetView) ;

  // create the button to rotate colors
  AWPushButton * pushToRotate = new AWPushButton (AWRect (20, 20, 100, 50), "Rotate", AWFont (Lucida_Grande24)) ;
  pushToRotate->setAction (rotate) ;
  addView (pushToRotate) ;
}

void loop() {
  AWContext::handleTouchAndDisplay () ;
}
