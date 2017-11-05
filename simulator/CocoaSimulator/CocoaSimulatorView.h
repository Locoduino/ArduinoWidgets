//
//  CocoaSimulatorView.h
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#import "AWColor.h"
#import <Cocoa/Cocoa.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@class AppDelegate ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@interface CocoaSimulatorView : NSView {
  @private int mWidth ;
  @private int mHeight ;
  @private NSInteger mZoom ;
  @private uint32_t * mScreenBuffer ;
  @private BOOL * mScreenChangeBuffer ;
  @private uint32_t mColor ;
  @private uint32_t mBackColor ;
  @private AppDelegate * mAppDelegate ;
  @private CGImageRef mImageRef ;
  @private uint32_t * mImageBuffer ;
}

- (void) setScreenWidth: (const int) inWidth
         screenHeight: (const int) inHeight
         backgroundScreenColor : (const AWColor) inColor
         applicationDelegate: (AppDelegate *) inAppDelegate ;

- (void) setColor: (const AWColor) inColor ;

- (void) setPixelAtX : (int) inX atY: (int) inY ;

- (void) setZoom:(NSInteger) inZoom ;
@end

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
