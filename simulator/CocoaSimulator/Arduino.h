//
//  AWContext.h
//  graphic
//
//  Created by Pierre Molinaro on 05/04/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef COCOA_ARDUINO_DEFINED
#define COCOA_ARDUINO_DEFINED

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   THIS FILE SHOULD ONLY BE USED FOR MAC OS SIMULATOR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  error "This file should only be used with the Cocoa Simulator"
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include <stdio.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

unsigned millis (void) ; // Declared here, implemented only in Simulator in Cocoa

void beep (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#endif