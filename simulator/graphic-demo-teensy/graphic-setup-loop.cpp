//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "graphic-setup-loop.h"
#include "AW-assertion.h"
#include "AWRect.h"
#include "AWContext.h"
#include "AWColor.h"
#include "AWLabel.h"
#include "AWSwitch.h"
#include "AWPushButton.h"
#include "AWListView.h"
#include "AWLine.h"
#include "AWAutoLabel.h"
#include "AWTabView.h"
#include "AWAlert.h"
#include "AWSegmentedControl.h"
#include "AWSlider.h"
#include "AWDynamicSlider.h"
#include "AWArrowPushButton.h"
#include "AWKeyboard.h"

#include "AWFont-Geneva9.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "Arduino.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  FOR DISPLAYING FREE RAM
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// https://forum.pjrc.com/threads/23256-Get-Free-Memory-for-Teensy-3-0

extern "C" char* sbrk(int incr);

static size_t freeRAM (void) {
  int top; // For getting stack address
  return ((size_t) &top) - ((size_t) sbrk(0)) ;
}

//······················································································································

static size_t gFreeRam ;
static AWAutoLabel * gFreeRamLabel ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AWLabel * gLabel1 ;
static AWLabel * gLabel2 ;
static AWAutoLabel * gLabel3 ;
static AWSwitch * gSwitch ;
static AWTabView * gTabView ;
static AWSegmentedControl * gSegmentedControl ;
static AWSlider * gSliderH;
static AWSlider * gSliderV;
static AWDynamicSlider * gDynSliderV;
static AWAutoLabel * gSliderHLabel;
static AWAutoLabel * gSliderVLabel;
static AWAutoLabel * gDynamicSliderVLabelTarget;
static AWAutoLabel * gDynamicSliderVLabelCurrent;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void switchAction (AWView * inSender) {
  gLabel2->setVisibility (gSwitch->checked ()) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//--- View and subviews

static AWView * gBaseView ;

//······················································································································

static void leftButtonAction (AWView * inSender) {
  gBaseView->translateBy (-20, 0) ;
}

//······················································································································

static void rightButtonAction (AWView * inSender) {
  gBaseView->translateBy (20, 0) ;
}

//······················································································································

static void upButtonAction (AWView * inSender) {
  gBaseView->translateBy (0, 20) ;
}

//······················································································································

static void downButtonAction (AWView * inSender) {
  gBaseView->translateBy (0, -20) ;
}

//·············································································
static void updateHSliderLabelAction (AWView * inSender)
{
  gSliderHLabel->setTitle(String(((AWSlider *)inSender)->knobPosition()));
}

//·············································································
static void updateVSliderLabelAction (AWView * inSender)
{
  gSliderVLabel->setTitle(String(((AWSlider *)inSender)->knobPosition()));
}

//·············································································
static void updateDynamicVSliderLabelAction (AWView * inSender)
{
  gDynamicSliderVLabelTarget->setTitle(String(((AWDynamicSlider *)inSender)->knobPosition()));
  gDynamicSliderVLabelCurrent->setTitle(String(((AWDynamicSlider *)inSender)->dynamicKnobPosition()));
}


//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// DragMeButton
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

class DragMeButton : public AWPushButton {
//--- Constructor
  public : DragMeButton (const AWPoint & inRelativeBaselineOrigin,
                         const AWInt inWidth) :
  AWPushButton (inRelativeBaselineOrigin, inWidth, "Drag me!"),
  mTouchLocation () {
  }

//--- Touch down
  public : virtual void touchDown (const AWPoint & inPoint) {
    mTouchLocation = inPoint ;
    super::touchDown (inPoint) ;
  }

//--- Touch move
  public : virtual void touchMove (const AWPoint & inPoint) {
    const AWInt dx = inPoint.x - mTouchLocation.x ;
    const AWInt dy = inPoint.y - mTouchLocation.y ;
    mTouchLocation = inPoint ;
    super::touchMove (inPoint) ;
    translateBy (dx, dy) ;
  }

//--- Touch location
  private : AWPoint mTouchLocation ;

//--- For calling super class instance methods
   private : typedef AWPushButton super ;
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Ellipse view
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

class EllipseView : public AWView {
//--- Constructor
  public : EllipseView (const AWRect & inRelativeFrame,
                        const AWColor & inBackColor,
                        const AWColor & inEllipseColor) :
  AWView (inRelativeFrame, inBackColor),
  mEllipseColor (inEllipseColor) {
  }

//--- Draw
  public : virtual void drawInRegion (const AWRegion & inDrawRegion) const {
  //--- Draw background
    super::drawInRegion (inDrawRegion) ;
  //--- Draw ellipse
    AWRect r = absoluteFrame () ;
    r.inset (1, 1) ;
    AWContext::setColor (mEllipseColor) ;
    r.fillOvalInRegion (inDrawRegion) ;
  }

//--- Private member
  private : AWColor mEllipseColor ;

//--- For calling super class instance methods
   private : typedef AWView super ;
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static AWListView * gListView ;
static AWAutoLabel * gSelectedItemLabel ;
static AWPushButton * gRemoveItemLabel ;
static AWSwitch * gHasBadgeSwitch ;

static void listSelectionAction (AWView * inSender) {
  const AWInt selectedItemIndex = gListView->selectedItemIndex () ;
  gSelectedItemLabel->setTitle (String (selectedItemIndex)) ;
  gRemoveItemLabel->setEnabled (selectedItemIndex >= 0) ;
  gHasBadgeSwitch->setEnabled (selectedItemIndex >= 0) ;
  gHasBadgeSwitch->setChecked (gListView->hasBadgeAtIndex (selectedItemIndex)) ;
  gTabView->setBadgeAtIndex (3, gListView->hasBadge ()) ;
}

static void listRemoveAction (AWView * inSender) {
  gListView->removeItemAtIndex (gListView->selectedItemIndex ()) ;
}

static void removeButtonAction (AWView * inSender) {
  presentAlert ("Êtes-vous sûr de bien vouloir supprimer ?", NULL, listRemoveAction, -1) ;
}

static void hasBadgeSwitchAction (AWView * inSender) {
  const AWInt selectedItemIndex = gListView->selectedItemIndex () ;
  if (selectedItemIndex >= 0) {
    gListView->setBadgeAtIndex (selectedItemIndex, gHasBadgeSwitch->checked ()) ;
  }
  gTabView->setBadgeAtIndex (3, gListView->hasBadge ()) ;
}

static void segmentedControlAction (AWSegmentedControl * inSender, const AWInt inHilitedTabIndex) {
//  beep () ;
}

AWLabel *gKeyboardResultLabel;

static void keyboardCallback (const String &inText, const int inTag)
{
  gKeyboardResultLabel->setTitle (inText) ;
}

static void showKbd ( AWView * inSender )
{
  launchKeyboard(String(""), 12, keyboardCallback) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// graphicSetup
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void graphicSetup (void) {
//---------------------- Configure
  AWContext::begin (kOrientationLandscape, // This value is not used by Mac OS X simulator
                    800, // Screen width
                    480, // Screen height
                    true, // true ⇔ X is flipped (this value is not used by Mac OS X simulator)
                    false) ; // true ⇔ Y is flipped (this value is not used by Mac OS X simulator)
//--- Displaying Timer
  gLabel1 = new AWLabel (AWPoint (1, 80), 100, kAWAlignmentCenter, "") ;
  gLabel1->setBackColor (AWColor::gray ()) ;
  gLabel1->setTextColor (AWColor::red ()) ;
  addView (gLabel1) ;
  gLabel2 = new AWLabel (AWPoint (1, 95), 100, kAWAlignmentRight, "") ;
  gLabel2->setBackColor (AWColor::darkGray ()) ;
  gLabel2->setTextColor (AWColor::orange ()) ;
  addView (gLabel2) ;
  gLabel3 = new AWAutoLabel (AWPoint (1, 110), "") ;
  gLabel3->setBackColor (AWColor::green ()) ;
  addView (gLabel3) ;
  gSwitch = new AWSwitch (AWPoint (102, 95), "Visible") ;
  gSwitch->setBackColor (AWColor::green ()) ;
  gSwitch->setAction (switchAction) ;
  addView (gSwitch) ;
  gSwitch->sendAction () ;

//---------------------- List view
  gRemoveItemLabel = new AWPushButton (AWPoint (170, 175), 100, "Remove") ;
  gRemoveItemLabel->setAction (removeButtonAction) ;
  addView (gRemoveItemLabel) ;
  gListView = new AWListView (AWRect (5, 140, 160, 305), "Ma liste") ;
  addView (gListView) ;
  gListView->append ("premier") ;
  gListView->append ("deuxième") ;
  gListView->append ("œil de bœuf") ;
  gListView->append ("troisième") ;
  gListView->append ("ŒUVRE") ;
  gListView->append ("quatrième") ;
  gListView->append ("cinquième") ;
  gListView->append ("sixième") ;
  gListView->append ("septième") ;
  gListView->append ("huitième") ;
  gListView->append ("neuvième") ;
  gListView->append ("dixième") ;
  gListView->append ("onzième") ;
  gListView->append ("12") ;
  gListView->append ("13") ;
  gListView->append ("quatorzième") ;
  gSelectedItemLabel = new AWAutoLabel (AWPoint (170, 145), "") ;
  addView (gSelectedItemLabel) ;
  gHasBadgeSwitch = new AWSwitch (AWPoint (170, 240), "Has Badge") ;
  gHasBadgeSwitch->setAction (hasBadgeSwitchAction) ;
  addView (gHasBadgeSwitch) ;
  gListView->setAction (listSelectionAction) ;

//---------------------- Line
  AWView * view = new AWLine (AWPoint (205, 205), AWPoint (170, 220)) ;
  addView (view) ;

//--- Ellipse
  view = new EllipseView (AWRect (1, 1, 75, 53), AWColor::gray (), AWColor::orange ()) ;
  addView (view) ;
  view = new EllipseView (AWRect (80, 1, 70, 50), AWColor::gray (), AWColor::orange ()) ;
  addView (view) ;

//---------------------- Tab view
  gTabView = new AWTabView (AWRect (300, 40, 450, 200)) ;
  view = new AWView (AWRect (0, 0, 100, 100), AWColor::brown ()) ;
  gTabView->addTab ("Premier", view) ;
  view = new AWView (AWRect (10, 10, 100, 100), AWColor::yellow ()) ;
  gTabView->addTab ("gDeuxième", view) ;
  view = new AWView (AWRect (20, 20, 100, 100), AWColor::cyan ()) ;
  gTabView->addTab ("Troisième", view) ;
  view = new AWView (AWRect (30, 30, 100, 100), AWColor::purple ()) ;
  gTabView->addTab ("Quatrième trop long", view) ;
  addView (gTabView) ;

//---------------------- Segmented control
  gSegmentedControl = new AWSegmentedControl (AWPoint (440, 100), 300) ;
  gSegmentedControl->addTab ("Premier") ;
  gSegmentedControl->addTab ("Deuxième") ;
  gSegmentedControl->addTab ("Troisième") ;
  //  gSegmentedControl->setEnabled (false) ;
  gSegmentedControl->setSegmentedControlAction (segmentedControlAction) ;
  addView (gSegmentedControl) ;
  
//---------------------- View and subviews
  gBaseView = new AWView (AWRect (550, 150, 150, 95), awkBackColor) ;
  addView (gBaseView) ;
  view = new AWPushButton (AWPoint (10, 40), 60, "Left") ;
  view->setAction (leftButtonAction) ;
  gBaseView->addSubView (view) ;
  view = new AWPushButton (AWPoint (80, 40), 60, "Right") ;
  view->setAction (rightButtonAction) ;
  gBaseView->addSubView (view) ;
  view = new AWPushButton (AWPoint (45, 10), 60, "Down") ;
  view->setAction (downButtonAction) ;
  gBaseView->addSubView (view) ;
  view = new AWPushButton (AWPoint (45, 70), 60, "Up") ;
  view->setAction (upButtonAction) ;
  gBaseView->addSubView (view) ;
  
  //---------------------- Drag me button
  view = new DragMeButton (AWPoint (190, 100), 100) ;
  addView (view) ;
  
  gSliderH = new AWSlider(AWPoint(300,200),200,kHorizontal) ;
  gSliderH->setHowManyScales( 20 ) ;
  addView(gSliderH) ;
  gSliderHLabel = new AWAutoLabel ( AWPoint( 310, 235), "50" ) ;
  gSliderH->setAction(updateHSliderLabelAction) ;
  addView(gSliderHLabel);
  
  gSliderV = new AWSlider(AWPoint(725,140),200,kVertical) ;
  addView(gSliderV) ;
  gSliderVLabel = new AWAutoLabel ( AWPoint( 725, 345 ), "50" ) ;
  gSliderV->setAction(updateVSliderLabelAction) ;
  addView(gSliderVLabel) ;
  
  gDynSliderV = new AWDynamicSlider(AWPoint(765,140),200,kVertical) ;
  addView(gDynSliderV) ;
  gDynamicSliderVLabelCurrent = new AWAutoLabel ( AWPoint( 765, 345), "50") ;
  gDynamicSliderVLabelTarget = new AWAutoLabel ( AWPoint( 765, 365), "50") ;
  gDynSliderV->setAction(updateDynamicVSliderLabelAction) ;
  addView(gDynamicSliderVLabelCurrent) ;
  addView(gDynamicSliderVLabelTarget) ;
  
//--- Arrow button
  AWArrowPushButton *bt2 = new AWArrowPushButton( AWRect(750,0,50,80), kUpArrow, AWColor::blue() ) ;
  bt2->setAction (showKbd) ;
  addView (bt2) ;
  
  gKeyboardResultLabel = new AWLabel (AWPoint(400,365), 200, kAWAlignmentLeft, String("Saisie clavier"), AWFont (Geneva9)) ;
  addView (gKeyboardResultLabel) ;
//--- Free ram
  gFreeRamLabel = new AWAutoLabel (AWPoint (0, 450), "") ;
  addView (gFreeRamLabel) ;

//---------------------- Send actions for synchronizing user interface
  gListView->sendAction () ;

//---------------------- Calibrate touch
//  AWContext::calibrateTouch () ;
//---------------------- Debug touch
// For enabling touch debug, uncomment the following line
//  AWContext::debugTouch (AWColor::red ()) ;
// if this line is uncommented, when a touch occurs, event is not sent to widgets, but a small square of the given
// color is drawn at touch location. This enables you to adjust the convertTouchPointToScreenPoint function
// (see convertTouchPointToScreenPoint.cpp) that converts touch location to actual screen location.
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// graphicLoop
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static unsigned gDeadLine ;
static unsigned gDynamicDeadLine ;
static unsigned gCount ;

//······················································································································

void graphicLoop (void) {
  if (gDeadLine <= millis ()) {
    gDeadLine += 1000 ;
    gCount ++ ;
    gLabel1->setTitle (String (gCount)) ;
    gLabel2->setTitle (String (gCount)) ;
    gLabel3->setTitle (String (gCount % 20)) ;
  }
  
  if (gDynamicDeadLine <= millis()) {
    gDynamicDeadLine += 100;
    AWInt targetPos = gDynSliderV->knobPosition();
    AWInt currentPos = gDynSliderV->dynamicKnobPosition();
    if (currentPos < targetPos) {
      gDynSliderV->setDynamicKnobPosition(currentPos + 1);
    }
    else if (currentPos > targetPos) {
      gDynSliderV->setDynamicKnobPosition(currentPos - 1);
    }
  }
  
  const size_t newFreeRam = freeRAM () ;
  if (gFreeRam != newFreeRam) {
    gFreeRam = newFreeRam ;
    gFreeRamLabel->setTitle ("Free Ram: " + String (gFreeRam)) ;
  }
//--- For flashing updates; has no effect in Cocoa Simulator.
//  AWContext::flashUpdate (AWColor::yellow ()) ;
//--- Last
  AWContext::handleTouchAndDisplay () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
