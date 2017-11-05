//
//  CocoaSimulatorView.m
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#import "CocoaSimulatorView.h"
#import "AppDelegate.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// https://developer.apple.com/library/content/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-CJBECCFG

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const int COMPONENTS_PER_PIXELS = sizeof (uint32_t) ;
static const int BITS_PER_COMPONENTS = 8 ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

@implementation CocoaSimulatorView

//······················································································································

- (void) setScreenWidth: (const int) inWidth
         screenHeight: (const int) inHeight
         backgroundScreenColor : (const AWColor) inBackColor
         applicationDelegate: (AppDelegate *) inAppDelegate {
  mAppDelegate = inAppDelegate ;
  mWidth = inWidth ;
  mHeight = inHeight ;
//--- Restore zoom from preferences
  NSUserDefaults * ud = [NSUserDefaults standardUserDefaults] ;
  mZoom = [ud integerForKey:@"ZOOM"] ;
  if (mZoom < 1) {
    mZoom = 1 ;
  }else if (mZoom > 10) {
    mZoom = 10 ;
  }
//--- Resize view
  NSRect currentFrame ;
  currentFrame.origin.x = 0.0 ;
  currentFrame.origin.y = 0.0 ;
  currentFrame.size.width = ((CGFloat) mZoom * mWidth) ;
  currentFrame.size.height = ((CGFloat) mZoom * mHeight) ;
  self.frame = currentFrame ;
//--- Set window title
  self.window.title = [NSString stringWithFormat:@"🖥 %d x %d", mWidth, mHeight] ;
//--- Make this view first responder
  self.window.acceptsMouseMovedEvents = YES ;
  [self.window makeFirstResponder:self] ;
//---
  const uint32_t red = inBackColor.redComponent () ;
  const uint32_t green = inBackColor.greenComponent () ;
  const uint32_t blue = inBackColor.blueComponent () ;
  mBackColor = red | (green << 8) | (blue << 16) ;
//--- Alloc data buffers
  const size_t pixelCount = (size_t) (inWidth * inHeight) ;
  mScreenBuffer = (uint32_t *) malloc (pixelCount * sizeof (uint32_t)) ;
  mScreenChangeBuffer = (BOOL *) malloc (pixelCount * sizeof (BOOL)) ;
  for (size_t i=0 ; i<pixelCount ; i++) {
    mScreenBuffer [i] = mBackColor ;
    mScreenChangeBuffer [i] = YES ;
  }
}

//······················································································································

- (void) setColor: (const AWColor) inColor {
  const uint32_t red = inColor.redComponent () ;
  const uint32_t green = inColor.greenComponent () ;
  const uint32_t blue = inColor.blueComponent () ;
  mColor = red | (green << 8) | (blue << 16) ;
// mForeColor --> bits 7 ... 0: red component, 15 ... 8: green component, 23 ... 16: blue component
}

//······················································································································

- (void) setMasterPixelAtX : (int) inX atY: (int) inY {
  const int idx = inY * mWidth + inX ;
  if (mScreenBuffer [idx] != mColor) {
    mScreenBuffer [idx] = mColor ;
    mScreenChangeBuffer [idx] = true ;
  }
}

//······················································································································

- (BOOL) pixelDidChangeAtX : (int) inX atY: (int) inY {
  const int idx = inY * mWidth + inX ;
  return mScreenChangeBuffer [idx] ;
}

//······················································································································

- (void) ackChangeAtX : (int) inX atY: (int) inY {
  const int idx = inY * mWidth + inX ;
  mScreenChangeBuffer [idx] = NO ;
}

//······················································································································

- (uint32_t) masterPixelAtX : (int) inX atY: (int) inY {
  const int idx = inY * mWidth + inX ;
  return mScreenBuffer [idx] ;
}

//······················································································································

- (void) setPixelAtX : (int) inX atY: (int) inY {
  if ((inX >= 0) && (inX < mWidth) && (inY >= 0) && (inY < mHeight)) {
  //--- Update master buffer
    [self setMasterPixelAtX:inX atY:inY] ;
  //--- Tell image did change
    CGImageRelease (mImageRef) ;
    mImageRef = NULL ;
  }else{
    NSLog (@"Pixel outside screen at (%d, %d)", inX, inY) ;
    NSBeep () ;
  }
}

//······················································································································

- (void) computeImage {
  if (mImageRef == NULL) {
    const size_t imageBufferPixelCount = (size_t) (mWidth * mHeight * mZoom * mZoom) ;
    const size_t imageBufferByteSize = imageBufferPixelCount * COMPONENTS_PER_PIXELS ;
  //--- Allocate image buffer
    if (NULL == mImageBuffer) {
      mImageBuffer = (uint32_t *) malloc (imageBufferByteSize) ;
    //--- Init image buffer
      for (size_t i=0 ; i<imageBufferPixelCount ; i++) {
        mImageBuffer [i] = mBackColor ;
      }
    //--- Tell all pixels should be redrawn
      for (int i=0 ; i<(mWidth * mHeight) ; i++) {
        mScreenChangeBuffer [i] = YES ;
      }
    }
  //--- Enter data in image buffer (only modified pixels)
    const int start = (int) (mZoom > 4) ;
    for (int x=0 ; x<mWidth ; x++) {
      for (int y=0 ; y<mHeight; y++) {
        if ([self pixelDidChangeAtX:x atY:y]) {
          [self ackChangeAtX:x atY:y] ;
          const uint32_t color = [self masterPixelAtX:x atY:y] ;
          const int flippedY = mHeight - y - 1 ;
          for (int dy = start ; dy < mZoom ; dy++) {
            for (int dx = start ; dx < mZoom ; dx++) {
              mImageBuffer [(flippedY * mZoom + dy) * mZoom * mWidth + x * mZoom + dx] = color ;
            }
          }
        }
      }
    }
  //--- Build image
    if (NULL == mImageRef) {
      CGDataProviderRef provider = CGDataProviderCreateWithData (NULL, mImageBuffer, imageBufferByteSize, NULL) ;
      CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB () ;
      mImageRef = CGImageCreate (
        (size_t) (mWidth * mZoom),
        (size_t) (mHeight * mZoom),
        BITS_PER_COMPONENTS,
        BITS_PER_COMPONENTS * COMPONENTS_PER_PIXELS,
        (size_t) (COMPONENTS_PER_PIXELS * mWidth * mZoom),
        rgb,
        (CGBitmapInfo) kCGImageAlphaNoneSkipLast,
        provider,
        NULL,
        false,
        kCGRenderingIntentDefault
      ) ;
      CGDataProviderRelease (provider);
      CGColorSpaceRelease (rgb);
    }
  }
}

//······················································································································

- (void) drawRect:(NSRect) inDirtyRect {
//  [super drawRect:inDirtyRect];
  [self computeImage] ;
  [NSGraphicsContext saveGraphicsState] ;
  NSGraphicsContext * currentContext = [NSGraphicsContext currentContext] ;
	currentContext.shouldAntialias = NO ;
  CGContextDrawImage (
    [currentContext CGContext],
    CGRectMake (0.0, 0.0, ((CGFloat) (mZoom * mWidth)), ((CGFloat) (mZoom * mHeight))),
    mImageRef
  ) ;
  [NSGraphicsContext restoreGraphicsState] ;
}

//······················································································································

- (void) mouseDown: (NSEvent *) inEvent {
  const NSPoint mouseDownLocation = [[self window].contentView convertPoint:inEvent.locationInWindow toView:self] ;
  const AWInt x = ((AWInt) mouseDownLocation.x) / mZoom ;
  const AWInt y = ((AWInt) mouseDownLocation.y) / mZoom ;
  [mAppDelegate mouseDownAtX:x y:y] ;
}

//······················································································································

- (void) mouseUp: (NSEvent *) inEvent {
  const NSPoint mouseDownLocation = [[self window].contentView convertPoint:inEvent.locationInWindow toView:self] ;
  const AWInt x = ((AWInt) mouseDownLocation.x) / mZoom ;
  const AWInt y = ((AWInt) mouseDownLocation.y) / mZoom ;
  [mAppDelegate mouseUpAtX:x y:y] ;
}

//······················································································································

- (void) mouseDragged: (NSEvent *) inEvent {
  const NSPoint mouseDownLocation = [[self window].contentView convertPoint:inEvent.locationInWindow toView:self] ;
  const AWInt x = ((AWInt) mouseDownLocation.x) / mZoom ;
  const AWInt y = ((AWInt) mouseDownLocation.y) / mZoom ;
  [mAppDelegate mouseDraggedAtX:x y:y] ;
  [self sendMouseLocation:inEvent.locationInWindow] ;
}

//······················································································································

- (void) sendMouseLocation: (NSPoint) inLocationInWindow {
  const NSPoint mouseDownLocation = [[self window].contentView convertPoint:inLocationInWindow toView:self] ;
  const int x = ((int) mouseDownLocation.x) / mZoom ;
  const int y = ((int) mouseDownLocation.y) / mZoom ;
  NSString * s = [NSString stringWithFormat:@"X: %d, Y:%d", x, y] ;
  [mAppDelegate mouseMovedString:s] ;
}

//······················································································································

- (void) mouseEntered: (NSEvent *) inEvent {
  [self sendMouseLocation:inEvent.locationInWindow] ;
}

//······················································································································

- (void) mouseMoved: (NSEvent *) inEvent {
  [self sendMouseLocation:inEvent.locationInWindow] ;
}

//······················································································································

- (void) mouseExited: (NSEvent *) inEvent {
  [mAppDelegate mouseMovedString:@""] ;
}

//······················································································································

- (void) setZoom:(NSInteger) inZoom {
  mZoom = inZoom ;
//--- Register zoom in preferences
  NSUserDefaults * ud = [NSUserDefaults standardUserDefaults] ;
  [ud setInteger:mZoom forKey:@"ZOOM"] ;
//--- Resize view
  NSRect currentFrame = self.frame ;
  currentFrame.size.width = ((CGFloat) mZoom * mWidth) ;
  currentFrame.size.height = ((CGFloat) mZoom * mHeight) ;
  self.frame = currentFrame ;
//--- Tell the image should be rebuilt
  CGImageRelease (mImageRef) ;
  mImageRef = NULL ;
  free (mImageBuffer) ;
  mImageBuffer = NULL ;
}

//······················································································································

- (void) dealloc {
  free (mScreenBuffer) ;
  free (mScreenChangeBuffer) ;
  free (mImageBuffer) ;
  CGImageRelease (mImageRef) ;
}

//······················································································································

@end

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

