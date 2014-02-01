AIA Control Board
=================

This is the firmware for akfrnswrth's Analog Integrated Amplifier's control board. 
Since this is a continuing work-in-progress, see the 
[most recent release] (https://github.com/akfrnswrth/AIA-Control-Board/releases)
for a working version.

This is currently a proof-of-concept; it doesn't interface with the the
preamp hardware yet.

Requirements
------------

This software requires specific tools to compile and specific hardware to run.

###Hardware

 * ATmega168 microcontroller
 * A 16-character M66004FP-based vacuum-fluorescent display connected to the 
   microcontroller's SPI bus and PB1 as chip select
 * Momentary buttons connecting PC0-PC6 to ground
 * (Optional) An LED attached to PB6 to indicate EEPROM writes
 * (Optional) A TSOP4838 or similar IR receiver on PB0
    * A Sony TV-compatible remote control to use the IR functionality

###Software
 * AVR toolchain (avr-gcc, avr-binutils, avrdude)
 * Basic GNU utilities (cp, make, rm, etc.)

This software is included in WinAVR.
