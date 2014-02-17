AIA Control Board software changelog
====================================
 
What's New in Version 0.3.0
---------------------------
 * Overhaul of buttons.c to get encoders to work properly
 * Support for local left, right, volup, and voldn buttons dropped to allow decoders
 * Menus moved to non-interrupt context to allow hardware control to run while in menus
 
Goals for Future Versions
-------------------------
 * Implement input selector control
 * Implement headphone detection and output switching
 * Implement automatic tone control behavior

Previous Versions
-----------------

###Version 0.2.1
 * Bugfix for Issue #15: "Tone control pots are all the way down at +0 instead
   of being centered"

###Version 0.2.0
 * Added changelog
 * Implemented digital potentiometer control
 * Added support for rotary encoders

###Version 0.1.0

 * Basic remote control capability added

###Version 0.0.2

 * Fully-functional menu interface
 * Proper brightness adjustment