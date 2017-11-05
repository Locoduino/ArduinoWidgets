//
//  AppDelegate.h
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#import <Cocoa/Cocoa.h>
#import "CocoaSimulatorView.h"
#import "AWColor.h"
#import "AWInt.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@interface AppDelegate : NSObject <NSApplicationDelegate> {

  @private CocoaSimulatorView * mSimulatorView ;
  @private NSTextField * mXYTrackerView ;
  @private NSPopUpButton * mZoomPopUpButton ;
  @private NSButton * mFlashUpdateCheckbox ;
  @private NSTimer * mTimer ;

}

@property (weak, atomic) IBOutlet NSWindow *window;

- (void) awakeFromNib ;

- (void) setPixelAtX : (AWInt) inX atY: (AWInt) inY ;

- (void) setColor: (const AWColor &) inColor ;

- (void) setScreenWidth: (const AWInt) inWidth
         screenHeight: (const AWInt) inHeight
         backgroundScreenColor : (const AWColor) inColor ;

- (void) setScreenNeedsDisplay ;
- (void) setScreenNeedsUpdateFlash ;
- (void) mouseDownAtX: (AWInt) inX y: (AWInt) inY ;
- (void) mouseDraggedAtX: (AWInt) inX y: (AWInt) inY ;
- (void) mouseUpAtX: (AWInt) inX y: (AWInt) inY ;
- (void) mouseMovedString: (NSString *) inString ;
- (bool) flashUpates ;
@end

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
