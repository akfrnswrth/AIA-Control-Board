AIA Control Board
=================

This is the firmware for my Analog Integrated Amplifier's control board.
Currently, the code should be considered under GPL v2 and authored by
akfrnswrth.

This is currently a proof-of-concept; it doesn't interface with the the
preamp hardware yet.

Requirements
------------

This software requires specific tools to compile and specific hardware to run.

###Hardware

 * ATmega168 microcontroller
 * A 16-character M66004FP-based vacuum-fluorescent display connected to the microcontroller's SPI bus and PB1 as chip select
 * (Optional) an LED attached to PB0 to indicate EEPROM writes
 * Momentary buttons connecting PC0-PC6 to ground

###Software
 * AVR toolchain (avr-gcc, avr-binutils, avrdude)
 * Basic GNU utilities (cp, make, rm, etc.)

This software is included in WinAVR.
