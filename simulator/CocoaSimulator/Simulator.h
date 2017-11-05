//
//  Simulator.h
//  CocoaSimulator
//
//  Created by Pierre Molinaro on 30/05/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   THIS FILE SHOULD USED ONLY FOR MAC OS SIMULATOR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef __APPLE__
  error "This file should not used only for the Cocoa Simulator"
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifndef SIMULATOR_FUNCTIONS_DEFINED
#define SIMULATOR_FUNCTIONS_DEFINED

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "AWColor.h"
#include "AWInt.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void simulatorDefineScreen (const AWInt inScreenWidth, const AWInt inScreenHeight, AWView * inScreenView) ;

void setSimulatorColor (const AWColor & inColor) ;

void simulatorDrawHLine (const AWInt inX, const AWInt inY, const AWInt inLength) ;

void simulatorDrawVLine (const AWInt inX, const AWInt inY, const AWInt inLength) ;

void simulatorUpdateDisplay (void) ;

void setSimulatorTouchDebugColor (const AWColor & inColor) ;
void setSimulatorSetRawTouchLocation (const AWPoint & inRawTouchLocation) ;



//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#endif
