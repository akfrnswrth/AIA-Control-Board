/*
 * main.c
 *
 * Created: 1/3/2014 7:05:58 PM
 *  Author: Ali
 * 
 * Control code for Analog Integrated Amplifier
 * Running on ATmega168
 *
 * Currently just shows UI, no hardware control implemented
 * TODO: 
 *  * HW control
 *  * Auto-brightness option
 *  * organize code within files
 *  * replace all strncpy with strlcpy
 *  * fix all occurrences of snprintf to always null-terminate
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "vfd.h"
#include "preamp.h"
#include "ui.h"
#include "main.h"

int main(void) {
	
	init();
	
    while(1) {
		uiloop();		// note that this puts cpu to sleep when nothing is happening
    }
}

void init() {
	DDRB = 0x00;		// start with non-destructive port settings
	PORTB = 0xff;		// (input w/ pullup)
	DDRC = 0x00;
	PORTC = 0xff;
	DDRD = 0x00;
	PORTD = 0xff;

	/*stdout = */vfdinit();	// start up VFD
	preinit();			// start up preamp controls
	uiinit();			// set up the UI and its interrupts
}

