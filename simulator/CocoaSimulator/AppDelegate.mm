//
//  AppDelegate.mm
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#import "AWRect.h"
#import "AWPushButton.h"
#import "AWContext.h"
#import "graphic-setup-loop.h"
#import "AW-assertion.h"
#import "AWView.h"
#import "Arduino.h"
#import "AWRegion.h"
#import "AW-settings.h"

#import "AppDelegate.h"
#import "Simulator.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AppDelegate * gDelegate ;
static NSDate * gStartDate ;
static AWView * gScreenView ;
static AWColor gTouchDebugColor ; // By default, transparent color --> no debug

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   millis: implemented on Simulator on Cocoa
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

unsigned millis (void) {
//--- NSTimeInterval is a double, expressed in seconds
  const NSTimeInterval timeInterval = [[NSDate date] timeIntervalSinceDate:gStartDate] ;
  const unsigned ms = unsigned (timeInterval * 1000.0) ;
  return ms ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SET DEBUG TOUCH COLOR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setSimulatorTouchDebugColor (const AWColor & inColor) {
  gTouchDebugColor = inColor ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SET COLOR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setSimulatorColor (const AWColor & inColor) {
  [gDelegate setColor:inColor] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SCREEN DEFINITION
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void simulatorDefineScreen (const AWInt inScreenWidth, const AWInt inScreenHeight, AWView * inScreenView) {
  gStartDate = [NSDate date] ;
  gScreenView = inScreenView ;
  [gDelegate setScreenWidth:inScreenWidth screenHeight:inScreenHeight backgroundScreenColor:AWColor::gray ()] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   DRAW LINES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void simulatorDrawHLine (const AWInt inX, const AWInt inY, const AWInt inLength) {
  for (AWInt x=inX ; x < (inX + inLength) ; x++) {
    [gDelegate setPixelAtX:x atY:inY] ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void simulatorDrawVLine (const AWInt inX, const AWInt inY, const AWInt inLength) {
  for (AWInt y=inY ; y < (inY + inLength) ; y++) {
    [gDelegate setPixelAtX:inX atY:y] ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void simulatorUpdateDisplay (void) {
  if ([gDelegate flashUpates] && AWView::viewsNeedDisplay ()) {
    AWContext::setColor (AWColor::yellow ()) ;
    gScreenView->drawAllViews (true) ;
    [gDelegate setScreenNeedsUpdateFlash] ;
  }
  if (AWView::viewsNeedDisplay ()) {
    gScreenView->drawAllViews (false) ;
    [gDelegate setScreenNeedsDisplay] ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AWView * gAlertView ;
static AWView * gTouchView ;
static AWPoint gScreenTouchPoint ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void simulatorHandleTouch (const AWPoint & inTouchScreenPoint) {
  if (gTouchDebugColor.isOpaque ()) { // Debug touch
    const AWColor savedColor = AWContext::color() ;
    AWContext::setColor (gTouchDebugColor) ;
    AWRect r (inTouchScreenPoint.x - 2, inTouchScreenPoint.y - 2, 5, 5) ;
    r.fillOvalInRegion (AWContext::screenRect()) ;
    r.inset (-2, -2) ;
    r.frameOvalInRegion (AWContext::screenRect()) ;
    AWContext::setColor (savedColor) ;
  }else{
    gScreenTouchPoint = inTouchScreenPoint ;
    setSimulatorSetRawTouchLocation (inTouchScreenPoint) ;
    if (NULL == gTouchView) {
      if (NULL != gAlertView) {
        gTouchView = gAlertView->viewAtPoint (gScreenTouchPoint) ;
      }else{
        gTouchView = gScreenView->viewAtPoint (gScreenTouchPoint) ;
      }
      if (NULL != gTouchView) {
        gTouchView->touchDown (gScreenTouchPoint) ;
      }
    }else{
      gTouchView->touchMove (gScreenTouchPoint) ;
    }
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void noTouch (void) {
  if (NULL != gTouchView) {
    gTouchView->touchUp (gScreenTouchPoint) ;
  }
  gTouchView = NULL ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    assertionFailure
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#if ASSERTION_GENERATION == 1
  void assertionFailure (const char * inMessage,
                         const uint32_t inValue,
                         const char * inSourceFile,
                         const int inSourceLine) {
    NSString * message = [NSString stringWithFormat:@"Error: %s\nAssociated value: %u\nFile: %s\nLine: %d",
      inMessage,
      inValue,
      inSourceFile,
      inSourceLine
    ] ;
    NSDictionary * userInfoDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
      message, NSLocalizedDescriptionKey,
      nil
    ] ;
    NSError * error = [NSError
      errorWithDomain:@"Simulator"
      code:inValue
      userInfo:userInfoDictionary
    ] ;
    [NSApp presentError:error] ;
  }
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void beep (void) {
  NSBeep () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   AppDelegate
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@implementation AppDelegate

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@synthesize window ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) awakeFromNib {
  gDelegate = self ;
//--- Add Autosave name
  NSWindow __strong * w = window ;
  w.frameAutosaveName = @"WINDOW_AUTOSAVE" ;
//--- Add scroll view
  NSRect r = w.contentView.bounds ;
  r.size.height -= 20.0 ;
  r = NSInsetRect (r, 20.0, 20.0) ;
  NSScrollView * sv = [[NSScrollView alloc] initWithFrame:r] ;
  sv.autoresizingMask = NSViewHeightSizable | NSViewWidthSizable ;
  sv.hasHorizontalScroller = YES ;
  sv.hasVerticalScroller = YES ;
  [w.contentView addSubview:sv] ;
//--- Add XY view
  r = w.contentView.bounds ;
  r.origin.y += r.size.height - 35.0 ;
  r.size.height = 20.0 ;
  r.origin.x += 20.0 ;
  r.size.width = 100 ;
  mXYTrackerView = [[NSTextField alloc] initWithFrame:r] ;
  mXYTrackerView.autoresizingMask = NSViewMinYMargin | NSViewMaxXMargin ;
  mXYTrackerView.editable = NO ;
  mXYTrackerView.bordered = NO ;
  mXYTrackerView.drawsBackground = NO ;
  [w.contentView addSubview:mXYTrackerView] ;
//--- Add Zoom popUp
  r = w.contentView.bounds ;
  r.origin.y += r.size.height - 35.0 ;
  r.size.height = 20.0 ;
  r.origin.x += 20.0 + 100.0 + 20.0 ;
  r.size.width = 100.0 ;
  mZoomPopUpButton = [[NSPopUpButton alloc] initWithFrame:r] ;
  mZoomPopUpButton.autoresizingMask = NSViewMinYMargin | NSViewMaxXMargin ;
  for (NSUInteger i=1 ; i<=10 ; i++) {
    NSString * s = [NSString stringWithFormat:@"x %lu", i] ;
    [mZoomPopUpButton addItemWithTitle:s] ;
  }
  mZoomPopUpButton.target = self ;
  mZoomPopUpButton.action = @selector(zoomChangeAction:) ;
  [w.contentView addSubview:mZoomPopUpButton] ;
//--------- Restore zoom from preferences
  NSUserDefaults * ud = [NSUserDefaults standardUserDefaults] ;
  NSInteger zoom = [ud integerForKey:@"ZOOM"] ;
  if (zoom < 1) {
    zoom = 1 ;
  }else if (zoom > 10) {
    zoom = 10 ;
  }
  [mZoomPopUpButton selectItemAtIndex:zoom - 1] ;
//--- Add Flash update checkbox
  r.origin.x += 20.0 + 100.0 + 20.0 ;
  r.size.width = 150.0 ;
  mFlashUpdateCheckbox = [[NSButton alloc] initWithFrame:r] ;
  [mFlashUpdateCheckbox setButtonType:NSButtonTypeSwitch] ;
  mFlashUpdateCheckbox.autoresizingMask = NSViewMinYMargin | NSViewMaxXMargin ;
  mFlashUpdateCheckbox.title = @"Flash Updates" ;
  mFlashUpdateCheckbox.target = self ;
  mFlashUpdateCheckbox.action = @selector (flashUpdatesCheckBoxAction:) ;
  [w.contentView addSubview:mFlashUpdateCheckbox] ;
//--------- Restore checkbox state from preferences
  const NSInteger checkboxState = [ud integerForKey:@"FLASH_UPDATES"] ;
  [mFlashUpdateCheckbox setState:checkboxState] ;
//--- Add simulator view
  r.origin.x = 0.0 ;
  r.origin.y = 0.0 ;
  r.size.width = 20.0 ;
  r.size.height = 20.0 ;
  mSimulatorView = [[CocoaSimulatorView alloc] initWithFrame:r] ;
  sv.documentView = mSimulatorView ;
//--- Timer
  mTimer = [NSTimer
    timerWithTimeInterval:0.1
    target:self
    selector:@selector (periodicAction)
    userInfo:NULL
    repeats:YES
  ] ;
  [[NSRunLoop currentRunLoop] addTimer:mTimer forMode:NSDefaultRunLoopMode] ;
  graphicSetup () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) zoomChangeAction: (id) inSender {
  const NSInteger zoom = mZoomPopUpButton.indexOfSelectedItem + 1 ;
  [mSimulatorView setZoom:zoom] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) flashUpdatesCheckBoxAction: (id) inSender {
  NSUserDefaults * ud = [NSUserDefaults standardUserDefaults] ;
  [ud setInteger:mFlashUpdateCheckbox.state forKey:@"FLASH_UPDATES"] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) periodicAction {
  graphicLoop () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) setScreenWidth: (const AWInt) inScreenWidth
         screenHeight: (const AWInt) inScreenHeight
         backgroundScreenColor : (const AWColor) inColor {
  [mSimulatorView
    setScreenWidth:inScreenWidth
    screenHeight:inScreenHeight
    backgroundScreenColor:inColor
    applicationDelegate:self
  ] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) setPixelAtX: (AWInt) inX atY: (AWInt) inY {
  [mSimulatorView setPixelAtX:inX atY:inY] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) setColor: (const AWColor &) inColor {
  [mSimulatorView setColor:inColor] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) setScreenNeedsUpdateFlash {
  [mSimulatorView display] ;
  [[NSRunLoop currentRunLoop] runUntilDate:[NSDate date]] ;
  [NSThread sleepForTimeInterval:0.25] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) setScreenNeedsDisplay {
  [mSimulatorView setNeedsDisplay:YES] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) mouseDownAtX: (AWInt) inX y: (AWInt) inY {
  simulatorHandleTouch(AWPoint (inX, inY)) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) mouseDraggedAtX: (AWInt) inX y: (AWInt) inY {
  simulatorHandleTouch (AWPoint (inX, inY)) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) mouseUpAtX: (AWInt) inX y: (AWInt) inY {
  noTouch () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (void) mouseMovedString: (NSString *) inString {
  mXYTrackerView.stringValue = inString ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

- (bool) flashUpates {
  return mFlashUpdateCheckbox.state ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@end
