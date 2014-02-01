/*
 * main.c - AIA Control Board firmware
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
 * Intended to run on ATMEGA168 
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
#include "buttons.h"

static void init() {
	DDRB = 0x00;		// start with non-destructive port settings
	PORTB = 0xff;		// (input w/ pullup)
	DDRC = 0x00;
	PORTC = 0xff;
	DDRD = 0x00;
	PORTD = 0xff;

	vfdinit();			// start up VFD
	preinit();			// start up preamp controls
	butinit();			// set up button sensing
	uiinit();			// set up the UI and its interrupts
}

int main(void) {
	
	init();
	
    while(1) {
		uiloop();		// note that this puts cpu to sleep when nothing is happening
    }
}

