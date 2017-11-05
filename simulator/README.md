# Cocoa Simulator

The CocoaSimulator is an OS X application that emulates screen and touch hardware. You can easily debug your project. Arduino projects and their simulation on OS X share many code, only some files are specific to Arduino or OS X.

For creating a new Simulator application:

- using Xcode, create a new project:
	* Application, "Cocoa"
	* Language: Objective C, no checkbox (do not use Storyboard, …)
- delete Xcode generated files AppDelegate.h and AppDelegate.m
- copy from the simulator source directory to your project source directory the following files :
	* <font color="marroon">AppDelegate.h</font>, <font color="marroon">AppDelegate.mm</font>
	* <font color="marroon">Arduino.h</font> (for compatibilty with Arduino)
	* <font color="marroon">CocoaSimulatorView.h</font>, <font color="marroon">CocoaSimulatorView.m</font>
	* <font color="marroon">WString.h</font>, <font color="marroon">WString.cpp</font> (Arduino String class)
- all files from graphic-test-teensy directory, but:
	* <font color="marroon">convertTouchPointToScreenPoint.h</font> and
	* <font color="marroon">convertTouchPointToScreenPoint.cpp</font> (only for Arduino)
	* <font color="marroon">graphic-test-teensy.ino</font>(only for Arduino, of course)
	* <font color="marroon">AWContext.cpp</font> (this is an implementation for Arduino, for a Cocoa Simulator implementation is provided in <font color="marroon">AppDelegate.mm</font>) 
	* <font color="marroon">touch-callbacks.h</font> (no call back in Cocoa Simulator)
