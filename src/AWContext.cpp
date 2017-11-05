//
//  AWContext.cpp
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "AWContext.h"
#include "AWColor.h"
#include "AWPushButton.h"
#include "AW-settings.h"

#ifndef __APPLE__
  #include "touch-calibration-values.h"
#endif

#ifdef __APPLE__
  #include "Simulator.h"
  #include <stdio.h>
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  #include <UTFT.h>
  extern UTFT myGLCD ;
#endif

#ifndef __APPLE__
  #include "AWTouch.h"
  extern AWTouch myTouch ;
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include <math.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static tOrientation gOrientation ;
static AWInt gScreenWidth ;
static AWInt gScreenHeight ;
static AWColor gColor ;
static bool gHorizontalFlip ;
static bool gVerticalFlip ;
static AWView * gScreenView ;
static AWColor gTouchDebugColor ; // By default, transparent color --> no debug

#ifndef __APPLE__
static AWNoTouchCallback gNoTouchCallback = NULL;
static AWCallbackTouchOn gCallbackTouchOn = NULL;
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   TOUCH CALIBRATION PARAMETERS
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const AWInt SIZE = 9 ;
static const AWInt OFFSET = 1 ;
static const AWInt DELTA = OFFSET + SIZE / 2 ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::debugTouch (const AWColor & inColor) {
  gTouchDebugColor = inColor ;
  #ifdef __APPLE__
    setSimulatorTouchDebugColor (inColor) ;
  #endif
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::begin (const tOrientation inOrientation,
                       const AWInt inScreenWidth,
                       const AWInt inScreenHeight,
                       const bool inHorizontalFlip,
                       const bool inVerticalFlip)
{
#ifndef __APPLE__
  myGLCD.InitLCD (inOrientation == kOrientationLandscape ? LANDSCAPE : PORTRAIT) ;
  myGLCD.clrScr () ;
#endif
  gOrientation = inOrientation;
  gScreenWidth = inScreenWidth ;
  gScreenHeight = inScreenHeight ;
  gHorizontalFlip = inHorizontalFlip ;
  gVerticalFlip = inVerticalFlip ;
  gScreenView = new AWView (AWRect (0, 0, inScreenWidth, inScreenHeight), awkBackColor) ;
  gScreenView->setOnScreen (true) ;
  gScreenView->setNeedsDisplay () ;
  #ifdef __APPLE__
    simulatorDefineScreen (inScreenWidth, inScreenHeight, gScreenView) ;
  #endif
  AWContext::handleTouchAndDisplay () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

AWRect AWContext::screenRect (void) {
  return AWRect (AWPoint (0, 0), AWSize (gScreenWidth, gScreenHeight)) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void addView (class AWView * inView) {
  gScreenView->addSubView (inView) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void addCenteredView (class AWView * inView) {
  gScreenView->addCenteredSubView (inView) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::setColor (const AWColor & inColor) {
  if (gColor != inColor) {
    gColor = inColor ;
    #ifdef __APPLE__
      setSimulatorColor (inColor) ;
    #else
    // color --> bits 4 ... 0: blue component, 10 ... 5: green component, 15 ... 11: red component
      const uint32_t red = inColor.redComponent () >> 3 ;
      const uint32_t green = inColor.greenComponent () >> 2 ;
      const uint32_t blue = inColor.blueComponent () >> 3 ;
      const uint32_t color = blue | (green << 5) | (red << 11) ;
      myGLCD.setColor (color) ;
    #endif
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

AWColor AWContext::color (void) {
  return gColor ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

bool AWContext::colorIsOpaque (void) {
  return gColor.isOpaque () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::drawHLine (const AWInt inX, const AWInt inY, const AWInt inLength) {
  if (inLength > 0) {
    #ifdef __APPLE__
      simulatorDrawHLine (inX, inY, inLength) ;
    #else
      const int X = gHorizontalFlip ? (gScreenWidth - inX - 1) : (inX - 1) ;
      const int Y = gVerticalFlip ? (gScreenHeight - inY - 1) : (inY - 1) ;
      const int length = gHorizontalFlip ? (-inLength) : inLength ;
      if (gOrientation == kOrientationPortrait) {
        myGLCD.drawVLine (Y, X, length) ;
      }else{
        myGLCD.drawHLine (X, Y, length) ;
      }
    #endif
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::drawVLine (const AWInt inX, const AWInt inY, const AWInt inLength) {
  if (inLength > 0) {
    #ifdef __APPLE__
      simulatorDrawVLine (inX, inY, inLength) ;
    #else
      const int X = gHorizontalFlip ? (gScreenWidth - inX - 1) : (inX - 1) ;
      const int Y = gVerticalFlip ? (gScreenHeight - inY - 1) : (inY - 1) ;
      const int length = gVerticalFlip ? (-inLength) : inLength ;
      if (gOrientation == kOrientationPortrait) {
        myGLCD.drawHLine (Y, X, length) ;
      }else{
        myGLCD.drawVLine (X, Y, length) ;
      }
    #endif
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   DIALOG
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AWView * gAlertView ;
static AWAction gCancelAction ;
static AWPushButton * gOkButton ;
static AWAction gOkAction ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void dismissDialog (void) {
  gAlertView->removeFromSuperView () ;
  delete gAlertView ;
  gAlertView = NULL ;
  gCancelAction = NULL ;
  gOkButton = NULL ;
  gOkAction = NULL ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void cancelDialogAction (AWView * inSender) {
  if (NULL != gCancelAction) {
    gCancelAction (inSender) ;
  }
  dismissDialog () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void okDialogAction (AWView * inSender) {
  if (NULL != gOkAction) {
    gOkAction (inSender) ;
  }
  dismissDialog () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::internalPresentAlert (AWView * inAlert,
                                      AWPushButton * inCancelButton,
                                      AWPushButton * inOkButton,
                                      AWAction inOkAction) {
  gCancelAction = inCancelButton->action () ;
  gOkButton = inOkButton ;
  gOkAction = inOkAction ;
//---
  inCancelButton->setAction (cancelDialogAction) ;
  inAlert->setAction (cancelDialogAction) ;
  gOkButton->setAction (okDialogAction) ;
//---
  gAlertView = inAlert ;
  addView (gAlertView) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AWPoint gRawTouchPoint ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

AWPoint AWContext::rawTouchPoint (void) {
  return gRawTouchPoint ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifdef __APPLE__
  void setSimulatorSetRawTouchLocation (const AWPoint & inRawTouchLocation) {
    gRawTouchPoint = inRawTouchLocation ;
  }
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   TOUCH (ONLY FOR ARDUINO)
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————


#ifndef __APPLE__
  typedef enum {
    Released,
    PressDetection,
    Pressed
  } PressState ;
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  static const unsigned TOUCH_ACQUISITION_TIME = 50 ; // In ms
  static PressState gPressState = Released ;
  static unsigned gClickTime ;
  static AWView * gViewUnderTouch ;
  static AWPoint gScreenTouchPoint ;
  static const int TOUCH_FILTER_ARRAY_SIZE = 10 ;
  static int gRawTouchArrayIndex ;
  static int gRawTouchCount ;
  static bool gHandleTouch ;
  static uint16_t gRawTouchPointArrayX [TOUCH_FILTER_ARRAY_SIZE] ;
  static uint16_t gRawTouchPointArrayY [TOUCH_FILTER_ARRAY_SIZE] ;
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  static AWPoint convertTouchPointToFlippedScreenPoint (const int inRawTouchX, const int inRawTouchY) {
  //--- Screen width and height, without orientation
    const int screenWidth  = (gOrientation == kOrientationPortrait) ? gScreenHeight : gScreenWidth  ;
    const int screenHeight = (gOrientation == kOrientationPortrait) ? gScreenWidth  : gScreenHeight ;
  //--- Largeur et hauteur de l'écran (en fait, moins un)
    const float W = (float) (screenWidth - 1) ;
    const float H = (float) (screenHeight - 1) ;
  //--- La distance des points de contrôle aux bords de l'écran
    const float delta = (float) DELTA ;
  //--- Le point en coordonnées de la dalle tactile que l'on cherche à convertir
    const float xTOUCH = (float) inRawTouchX ;
    const float yTOUCH = (float) inRawTouchY ;
  //--- Valeurs de ABCDx et ABCDy
    const float ABCDx = xA - xB + xC - xD ; // A - B + C - D
    const float ABCDy = yA - yB + yC - yD ; // A - B + C - D
  //--- Première estimation de u
    float u = 0.5 ;
  //--- Première estimation de v
    float v = 0.5 ;
  //--- Première estimation de la position écran
    float xSCREEN = v * (W - 2.0 * delta) + delta ;
    float ySCREEN = u * (H - 2.0 * delta) + delta ;
  //--- Calcul itératif de u et v
    const bool select = fabsf (yB - yA) > fabsf (yD - yA) ;
    bool loop = true ;
    while (loop) {
      if (select) {
        u = (yTOUCH - u * v * ABCDy - v * (yD - yA) - yA) / (yB - yA) ;
        v = (xTOUCH - u * v * ABCDx - u * (xB - xA) - xA) / (xD - xA) ;
      }else{
        u = (xTOUCH - u * v * ABCDx - v * (xD - xA) - xA) / (xB - xA) ;
        v = (yTOUCH - u * v * ABCDy - u * (yB - yA) - yA) / (yD - yA) ;
      }
      const float new_xSCREEN = v * (W - 2.0 * delta) + delta ;
      const float new_ySCREEN = u * (H - 2.0 * delta) + delta ;
      loop = (fabsf (new_xSCREEN - xSCREEN) > 1) || (fabsf (new_ySCREEN - ySCREEN) > 1) ;
      xSCREEN = new_xSCREEN ;
      ySCREEN = new_ySCREEN ;
    }
  //---
    const int ixSCREEN = (int) xSCREEN ;
    const int iySCREEN = (int) ySCREEN ;
  //--- Handle horizontal flip, and maintain X between 0 and screen width - 1
    int X = gHorizontalFlip ? (screenWidth - ixSCREEN - 1) : ixSCREEN ;
    if (X < 0) {
      X = 0 ;
    }else if (X >= screenWidth) {
      X = screenWidth - 1 ;
    }
  //--- Handle vertical flip, and maintain Y between 0 and screen height - 1
    int Y = gVerticalFlip ? (screenHeight - iySCREEN - 1) : iySCREEN ;
    if (Y < 0) {
      Y = 0 ;
    }else if (Y >= screenHeight) {
      Y = screenHeight - 1 ;
    }
  //---
    return (gOrientation == kOrientationPortrait) ? AWPoint (Y, X) : AWPoint (X, Y) ;
  }
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  static void quickSort (uint16_t inArray [], const int inLeft, const int inRight) {
    const int pivot = inArray [(inLeft + inRight) / 2] ;
    int i = inLeft ;
    int j = inRight ;
  //--- Partition
     while (i <= j) {
       while (inArray[i] < pivot) {
         i += 1 ;
       }
       while (inArray [j] > pivot) {
         j -= 1 ;
       }
       if (i <= j) {
         int tmp = inArray[i];
         inArray[i] = inArray[j];
         inArray[j] = tmp;
         i += 1 ;
         j -= 1 ;
       }
     }
   //--- Recursion
     if (inLeft < j) {
       quickSort (inArray, inLeft, j);
     }
     if (i < inRight) {
       quickSort (inArray, i, inRight);
     }
  }
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::handleTouchAndDisplay (void) {
  #ifdef __APPLE__
    simulatorUpdateDisplay () ;
  #else
  //--- Handle touch on Arduino
    const bool touch = myTouch.dataAvailable () ;
    if (touch) {
    //--- Read touch raw location
      uint16_t touchX ;
      uint16_t touchY ;
      myTouch.read (touchX, touchY) ;
      switch (gPressState) {
      case Released :
        gClickTime = millis () + TOUCH_ACQUISITION_TIME ;
        gRawTouchPointArrayX [0] = touchX ;
        gRawTouchPointArrayY [0] = touchY ;
        gRawTouchArrayIndex = 1 ;
        gRawTouchCount = 1 ;
        gPressState = PressDetection ;
        break ;
      case PressDetection :
        gRawTouchPointArrayX [gRawTouchArrayIndex] = touchX ;
        gRawTouchPointArrayY [gRawTouchArrayIndex] = touchY ;
        gRawTouchArrayIndex = (gRawTouchArrayIndex + 1) % TOUCH_FILTER_ARRAY_SIZE ;
        gRawTouchCount += 1 ;
        if ((gClickTime < millis ()) && (gRawTouchCount >= TOUCH_FILTER_ARRAY_SIZE)) {
          gPressState = Pressed ;
          gClickTime += TOUCH_ACQUISITION_TIME ;
          quickSort (gRawTouchPointArrayX, 0, TOUCH_FILTER_ARRAY_SIZE - 1) ;
          quickSort (gRawTouchPointArrayY, 0, TOUCH_FILTER_ARRAY_SIZE - 1) ;
          gRawTouchPoint.x = gRawTouchPointArrayX [TOUCH_FILTER_ARRAY_SIZE / 2] ;
          gRawTouchPoint.y = gRawTouchPointArrayY [TOUCH_FILTER_ARRAY_SIZE / 2] ;
          gRawTouchCount = 0 ;
          gScreenTouchPoint = convertTouchPointToFlippedScreenPoint (gRawTouchPoint.x, gRawTouchPoint.y) ;
        //---
          gHandleTouch = gCallbackTouchOn == NULL ? true : gCallbackTouchOn () ;
          if (gHandleTouch) {
            if (gTouchDebugColor.isOpaque ()) {
              setColor (gTouchDebugColor) ;
              AWRect r (gScreenTouchPoint.x - 2, gScreenTouchPoint.y - 2, 5, 5) ;
              r.fillOvalInRegion (AWContext::screenRect()) ;
              r.inset (-2, -2) ;
              r.frameOvalInRegion (AWContext::screenRect()) ;
            }else{
              if (NULL != gAlertView) {
                gViewUnderTouch = gAlertView->viewAtPoint (gScreenTouchPoint) ;
              }else{
                gViewUnderTouch = gScreenView->viewAtPoint (gScreenTouchPoint) ;
              }
              if (NULL != gViewUnderTouch) {
                gViewUnderTouch->touchDown (gScreenTouchPoint) ;
              }
            }
          }
        }
        break ;
      case Pressed :
        gRawTouchPointArrayX [gRawTouchArrayIndex] = touchX ;
        gRawTouchPointArrayY [gRawTouchArrayIndex] = touchY ;
        gRawTouchArrayIndex = (gRawTouchArrayIndex + 1) % TOUCH_FILTER_ARRAY_SIZE ;
        gRawTouchCount += 1 ;
        if ((gClickTime < millis ()) && (gRawTouchCount >= 100)) {
          gClickTime += TOUCH_ACQUISITION_TIME ;
          quickSort (gRawTouchPointArrayX, 0, TOUCH_FILTER_ARRAY_SIZE - 1) ;
          quickSort (gRawTouchPointArrayY, 0, TOUCH_FILTER_ARRAY_SIZE - 1) ;
          gRawTouchPoint.x = gRawTouchPointArrayX [TOUCH_FILTER_ARRAY_SIZE / 2] ;
          gRawTouchPoint.y = gRawTouchPointArrayY [TOUCH_FILTER_ARRAY_SIZE / 2] ;
          gRawTouchCount = 0 ;
          gScreenTouchPoint = convertTouchPointToFlippedScreenPoint (gRawTouchPoint.x, gRawTouchPoint.y) ;
        //---
          if (gHandleTouch) {
            if (gTouchDebugColor.isOpaque ()) {
              setColor (AWColor::blue ()) ;
              AWRect r (gScreenTouchPoint.x - 2, gScreenTouchPoint.y - 2, 5, 5) ;
              r.fillOvalInRegion (AWContext::screenRect()) ;
              r.inset (-2, -2) ;
              r.frameOvalInRegion (AWContext::screenRect()) ;
            }else if (NULL != gViewUnderTouch) {
              gViewUnderTouch->touchMove (gScreenTouchPoint) ;
            }
          }
        }
        break ;
      }
    }else{
      gPressState = Released ;
      if (gHandleTouch && (NULL != gViewUnderTouch) && !gTouchDebugColor.isOpaque ()) {
        gViewUnderTouch->touchUp (gScreenTouchPoint) ;
      }
      gViewUnderTouch = NULL ;
      if (gNoTouchCallback != NULL) gNoTouchCallback () ;
    }
  //--- Handle screen update
    if (AWView::viewsNeedDisplay ()) {
      gScreenView->drawAllViews (false) ;
    }
  #endif
}


//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::flashUpdate (const AWColor & inFlashColor) {
  #ifndef __APPLE__
    if (AWView::viewsNeedDisplay ()) {
      AWContext::setColor (inFlashColor) ;
      gScreenView->drawAllViews (true) ;
      delay (200) ;
    }
  #endif
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   CALIBRATE TOUCH VIEW
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

class CalibrateTouchView : public AWView {

//··· Constructor ······································································································

  public : CalibrateTouchView (void) :
  AWView (AWContext::screenRect (), AWColor::black ()),
  mPhase (0),
  mRawPoint (),
  mRawTestPoint () {
  }

//··· Draw method ······································································································

  public : virtual void drawInRegion (const AWRegion & inDrawRegion) const {
  //--- Background
    super::drawInRegion (inDrawRegion) ;
  //--- Title
    AWContext::setColor (AWColor::white ()) ;
    const AWRect screenRect = AWContext::screenRect () ;
    const char * title = "Touch Calibration" ;
    const AWInt x = screenRect.minX () + 40 ;
    AWInt y = screenRect.maxY () - awkDefaultFont.ascent () - 1 ;
    awkDefaultFont.drawStringInRegion (x, y, title, inDrawRegion) ;
  //--- Draw touch phases
    y -= 2 * awkDefaultFont.lineHeight () ;
    AWContext::setColor ((mPhase == 0) ? AWColor::blue () : AWColor::gray ()) ;
    awkDefaultFont.drawStringInRegion (x, y, "1 - Touch bottom left red square", inDrawRegion) ;
    y -= awkDefaultFont.lineHeight () ;
    AWContext::setColor ((mPhase == 1) ? AWColor::blue () : AWColor::gray ()) ;
    awkDefaultFont.drawStringInRegion (x, y, "2 - Touch top left red square", inDrawRegion) ;
    y -= awkDefaultFont.lineHeight () ;
    AWContext::setColor ((mPhase == 2) ? AWColor::blue () : AWColor::gray ()) ;
    awkDefaultFont.drawStringInRegion (x, y, "3 - Touch top right red square", inDrawRegion) ;
    y -= awkDefaultFont.lineHeight () ;
    AWContext::setColor ((mPhase == 3) ? AWColor::blue () : AWColor::gray ()) ;
    awkDefaultFont.drawStringInRegion (x, y, "4 - Touch bottom right red square", inDrawRegion) ;
    y -= awkDefaultFont.lineHeight () ;
    AWContext::setColor (AWColor::white ()) ;
    if (mPhase > 3) {
      y -= 2 * awkDefaultFont.lineHeight () ;
      const int flip = (gHorizontalFlip ? 2 : 0) | (gVerticalFlip ? 1 : 0) ; // 0: bottom left, 1: top left, 2: bottom right, 3:top right
      awkDefaultFont.drawStringInRegion (x, y, "Enter theses calibration values", inDrawRegion) ;
      y -= awkDefaultFont.lineHeight () ;
      awkDefaultFont.drawStringInRegion (x, y, "into \"touch-calibration-values.cpp\"", inDrawRegion) ;
      y -= awkDefaultFont.lineHeight () ;
      AWPoint p = mRawPoint [flip] ;
      awkDefaultFont.drawStringInRegion (x, y, "xA = " + String (p.x) + ", yA = " + String (p.y), inDrawRegion) ;
      y -= awkDefaultFont.lineHeight () ;
      p = (gOrientation == kOrientationPortrait) ? mRawPoint [flip ^ 2] : mRawPoint [flip ^ 1] ;
      awkDefaultFont.drawStringInRegion (x, y, "xB = " + String (p.x) + ", yB = " + String (p.y), inDrawRegion) ;
      y -= awkDefaultFont.lineHeight () ;
      p = mRawPoint [flip ^ 3] ;
      awkDefaultFont.drawStringInRegion (x, y, "xC = " + String (p.x) + ", yC = " + String (p.y), inDrawRegion) ;
      y -= awkDefaultFont.lineHeight () ;
      p = (gOrientation == kOrientationPortrait) ? mRawPoint [flip ^ 1] : mRawPoint [flip ^ 2] ;
      awkDefaultFont.drawStringInRegion (x, y, "xD = " + String (p.x) + ", yD = " + String (p.y), inDrawRegion) ;
    }
    float xSCREEN = 0.0 ;
    float ySCREEN = 0.0 ;
    if (mPhase > 4) {
    //--- Largeur et hauteur de l'écran (en fait, moins un)
      const float W = (float) (gScreenWidth - 1) ;
      const float H = (float) (gScreenHeight - 1) ;
    //--- The four control points
      const float xxA = (float) mRawPoint [0].x ;
      const float yyA = (float) mRawPoint [0].y ;
      const float xxB = (float) mRawPoint [1].x ;
      const float yyB = (float) mRawPoint [1].y ;
      const float xxC = (float) mRawPoint [3].x ;
      const float yyC = (float) mRawPoint [3].y ;
      const float xxD = (float) mRawPoint [2] .x ;
      const float yyD = (float) mRawPoint [2] .y ;
    //--- La distance des points de contrôle aux bords de l'écran
      const float delta = (float) DELTA ;
    //--- Le point en coordonnées de la dalle tactile que l'on cherche à convertir
      const float xTOUCH = (float) mRawTestPoint.x ;
      const float yTOUCH = (float) mRawTestPoint.y ;
    //--- Valeurs de ABCDx et ABCDy
      const float ABCDx = xxA - xxB + xxC - xxD ; // A - B + C - D
      const float ABCDy = yyA - yyB + yyC - yyD ; // A - B + C - D
    //--- Start values for u and v
      float u = 0.5 ;
      float v = 0.5 ;
    //--- First valuation of screen point coordinates
      xSCREEN = v * (W - 2 * delta) + delta ;
      ySCREEN = u * (H - 2 * delta) + delta ;
      // printf ("***** u %f, v %f xSCREEN %f ySCREEN %f\n", u, v, xSCREEN, ySCREEN) ;
    //--- Iterative computing of u, v, xSCREEN and ySCREEN
      const bool select = fabsf (yyB - yyA) > fabsf (yyD - yyA) ;
      bool loop = true ;
      while (loop) {
        if (select) {
          u = (yTOUCH - u * v * ABCDy - v * (yyD - yyA) - yyA) / (yyB - yyA) ;
          v = (xTOUCH - u * v * ABCDx - u * (xxB - xxA) - xxA) / (xxD - xxA) ;
        }else{
          u = (xTOUCH - u * v * ABCDx - v * (xxD - xxA) - xxA) / (xxB - xxA) ;
          v = (yTOUCH - u * v * ABCDy - u * (yyB - yyA) - yyA) / (yyD - yyA) ;
        }
        const float new_xSCREEN = v * (W - 2 * delta) + delta ;
        const float new_ySCREEN = u * (H - 2 * delta) + delta ;
        loop = (fabsf (new_xSCREEN - xSCREEN) > 1) || (fabsf (new_ySCREEN - ySCREEN) > 1) ;
        xSCREEN = new_xSCREEN ;
        ySCREEN = new_ySCREEN ;
        // printf ("u %f, v %f xSCREEN %f ySCREEN %f\n", u, v, xSCREEN, ySCREEN) ;
      }
      y -= 2 * awkDefaultFont.lineHeight () ;
      String s = "Screen test point (" ;
      s += String (xSCREEN) + ", " + String (ySCREEN) + ")" ;
      awkDefaultFont.drawStringInRegion (x, y, s, inDrawRegion) ;
    }
    if (mPhase >= 4) {
      y -= awkDefaultFont.lineHeight () ;
      AWContext::setColor (AWColor::blue ()) ;
      awkDefaultFont.drawStringInRegion (x, y, "You can touch any point for checking", inDrawRegion) ;
    }
  //--- Hilite corner
    switch (mPhase) {
    case 0 : // Bottom left
      drawCalibrationRectangle (DELTA, DELTA, inDrawRegion) ;
      break ;
    case 1 : // Top left
      drawCalibrationRectangle (DELTA, screenRect.maxY () - DELTA, inDrawRegion) ;
      break ;
    case 2 : // Top right
      drawCalibrationRectangle (screenRect.maxX () - DELTA, screenRect.maxY () - DELTA, inDrawRegion) ;
      break ;
    case 3 : // Bottom right
      drawCalibrationRectangle (screenRect.maxX () - DELTA, DELTA, inDrawRegion) ;
      break ;
    case 5 : { // Check point
      AWRect corner = screenRect ;
      corner.origin.x = ((AWInt) xSCREEN) - SIZE / 2 ;
      corner.origin.y = ((AWInt) ySCREEN) - SIZE / 2 ;
      corner.size.width = SIZE ;
      corner.size.height = SIZE ;
      AWContext::setColor (AWColor::red ()) ;
      corner.fillRectInRegion (inDrawRegion) ;
      } break ;
    default:
      break ;
    }
  }

//··· Draw a calibration rectangle ·····················································································

  private : void drawCalibrationRectangle (const AWInt inX, const AWInt inY, const AWRegion & inDrawRegion) const {
    AWContext::setColor (AWColor::red ()) ;
    const AWRect r (inX - SIZE / 2, inY - SIZE / 2, SIZE, SIZE) ;
    r.frameRectInRegion (inDrawRegion) ;
    AWPoint::strokeLineInRegion (inX - SIZE / 2 + 2, inY, inX + SIZE / 2 - 2, inY, inDrawRegion) ;
    AWPoint::strokeLineInRegion (inX, inY - SIZE / 2 + 2, inX, inY + SIZE / 2 - 2, inDrawRegion) ;
  }

//··· Touch up ·········································································································

  public : virtual void touchUp (const AWPoint & /* inScreenPoint */) {
    const AWPoint rawTouchPoint = AWContext::rawTouchPoint () ;
    switch (mPhase) {
    case 0 :
      mRawPoint [0] = rawTouchPoint ;  // 0: bottom left
      mPhase += 1 ;
      setNeedsDisplay () ;
      break ;
    case 1 :
      mRawPoint [1] = rawTouchPoint ; // 1: top left,
      mPhase += 1 ;
      setNeedsDisplay () ;
      break ;
    case 2 :
      mRawPoint [3] = rawTouchPoint ; // 3:top right
      mPhase += 1 ;
      setNeedsDisplay () ;
      break ;
    case 3 :
      mRawPoint [2] = rawTouchPoint ; // 2: bottom right
      mPhase += 1 ;
      setNeedsDisplay () ;
      break ;
    case 4 :
      mRawTestPoint = rawTouchPoint ;
      mPhase += 1 ;
      setNeedsDisplay () ;
      break ;
    case 5 :
      mRawTestPoint = rawTouchPoint ;
      setNeedsDisplay () ;
      break ;
    default:
      break ;
    }
  }

//··· Properties ·······································································································

  private : int mPhase ;
  private : AWPoint mRawPoint [4] ; // 0: bottom left, 1: top left, 2: top right, 3:bottom right
  private : AWPoint mRawTestPoint ;

//··· For calling super class instance methods ·········································································

  private : typedef AWView super ;
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void AWContext::calibrateTouch (void) {
  AWView * calibrateView = new CalibrateTouchView ;
  addView (calibrateView) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
