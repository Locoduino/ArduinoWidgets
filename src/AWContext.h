//
//  AWContext.h
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef GRAPHIC_CONTEXT_CLASS_DEFINED
#define GRAPHIC_CONTEXT_CLASS_DEFINED

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "WString.h"
#include "AWView.h"
#include "AWInt.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

class AWRect ;
class AWView ;
class AWColor ;
class AWFont ;
class AWPoint ;
class AWPushButton ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Callback function pointer type

typedef void (*AWNoTouchCallback)();
typedef bool (*AWCallbackTouchOn)();

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include <stdint.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void beep (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void addView (class AWView * inView) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void addCenteredView (class AWView * inView) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

typedef enum {kOrientationLandscape, kOrientationPortrait} tOrientation ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

class AWContext {
  public : static void begin (const tOrientation inOrientation,
                              const AWInt inScreenWidth,
                              const AWInt inScreenHeight,
                              const bool inHorizontalFlip,
                              const bool inVerticalFlip) ;
//--- Screen rect
  public : static AWRect screenRect (void) ;

//--- Color
  public : static void setColor (const AWColor & inColor) ;
  public : static AWColor color (void) ;
  public : static bool colorIsOpaque (void) ;

//--- Touch
  public : static void handleTouchAndDisplay (void) ;
  public : static void flashUpdate (const AWColor & inFlashColor) ;

  public : static void calibrateTouch (void) ;
  public : static AWPoint rawTouchPoint (void) ;
  public : static void debugTouch (const AWColor & inColor) ;
  public : static void handleTouch (const AWPoint & inScreenPoint) ;

//--- Dialog
  private : static void internalPresentAlert (AWView * inAlert,
                                              AWPushButton * inCancelButton,
                                              AWPushButton * inOkButton,
                                              AWAction inOkAction) ;

//--- Internal drawing methods
  public : static void drawHLine (const AWInt inX, const AWInt inY, const AWInt inLength) ;
  public : static void drawVLine (const AWInt inX, const AWInt inY, const AWInt inLength) ;

//--- Friend
  friend void presentAlert (const String & inTitle,
                            AWAction inCancelAction,
                            AWAction inOkAction,
                            const int inOkActionTag) ;
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#endif
