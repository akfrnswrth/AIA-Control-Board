/*
 * buttons.c - Handle button presses both local and remote 
 * Copyright (C) 2014 Ali Kocaturk <akfrnswrth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * TODO: remote code
 * Uses ISR(PCINT1_vect) for local and Timer1 for remote
 *
 */
 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "pins.h"
#include "buttons.h"

#define DEBOUNCE_TIME 40	// debounce time, in milliseconds

static char but_getlocalbutton();

static void (*but_intfunc)() = 0;// pointer to the set interrupt function

void butinit() {
	DDRC &= ~(PINC_BUTMASK);		// make sure all buttons are inputs
	PORTC |= PINC_BUTMASK;			// inputs with pullups, that is
}

void but_setint(void (*f)()) {
	PCICR |= 1<<PCIE1;				// enable pin-change interrupt
	PCMSK1 |= PCI1_MASK;			// and enable each button interrupt
	// TODO: Remote control interrupts
	
	but_intfunc = f;
}

/*
 * Waits for local or remote button press and debounces
 * Returns enum but_type of button pressed
 */
enum but_type but_getaction() {
    return but_getlocalbutton();
}

/*
 * Determines whether any buttons, local or remote, have been pressed.
 * Returns non-zero if any are pressed, zero if none pressed.
 */
uint8_t but_ispressed() {
	return (~PINC) & PINC_BUTMASK;	// only local buttons currently implemented
}

/*
 * Waits for a local button press, then
 * saves and debounces button input
 * Returns enum but_type of button pressed
 * Clears PCINT1 caused by button lifting
 */
static char but_getlocalbutton() {
	uint8_t pressedbyte = (~PINC) & PINC_BUTMASK;
	
	while(!pressedbyte) pressedbyte = (~PINC) & PINC_BUTMASK;	// fixes issues with turn-on bounce
	
	// wait for continuous DEBOUNCE_TIME ms of all buttons open
	for(uint8_t timeleft = DEBOUNCE_TIME; timeleft > 0; timeleft--) {
		if((PINC & PINC_BUTMASK) != PINC_BUTMASK) timeleft = DEBOUNCE_TIME;
		_delay_ms(1);
	}
	PCIFR = 1<<PCIF1;	// clear PCINT1 caused by switch lifting
	
	return pressedbyte;
}

ISR(PCINT1_vect) {
	// only run but_intfunc() if it has been set!
	if(but_intfunc) but_intfunc();
}