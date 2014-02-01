/* buttons.c - Handle button presses both local and remote 
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
 * Designed to work with 12-bit Sony IR protocol. No guarantees.
 *
 * TODO: remote code
 * TODO: increase compatibility with other clock frequencies
 * Uses ISR(PCINT1_vect) for local and Timer1 for remote
 * Needs F_CPU to be 1000000 Hz.
 */
 


#if F_CPU != 1000000UL
#error "F_CPU must be 100000 Hz. Other CPU frequencies are not yet supported."
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "pins.h"
#include "buttons.h"
#include "vfd.h"

#define DEBOUNCE_TIME 40	// debounce time, in milliseconds
#define REM_TIMEOUT 60000U	// max time between remote packets, in microseconds
#define REM_ADDRESS 1		// device address to listen for


static char but_getlocalbutton();
static uint8_t but_isremotepressed();
static uint8_t but_islocalpressed();
static enum but_type but_getremotebutton();

static void (*but_intfunc)() = 0;	// pointer to the set interrupt function

void butinit() {
	DDRB &= ~(1<<REM_RX);			// make sure IR receiver is input
	PORTB |= 1<<REM_RX;				// enable pullup
	DDRC &= ~(PINC_BUTMASK);		// make sure all buttons are inputs
	PORTC |= PINC_BUTMASK;			// inputs with pullups, that is
	
	TCCR1A = 0;						// Ensure timer is in 'normal' mode
	TCCR1B |= (1<<ICNC1)|(0<<ICES1)|(1<<CS10);	// Detect falling edge (start of timing), max count rate
	TIMSK1 = 0;
}

/*
 * Attaches a function to be called whenever a button interrupt occurs
 */
void but_setint(void (*f)()) {
	PCICR |= 1<<PCIE1;				// enable pin-change interrupt
	PCMSK1 |= PCI1_MASK;			// and enable each button interrupt
	TIMSK1 |= 1<<ICIE1;				// Enable input compare interrupt for remote receiver
	
	but_intfunc = f;
}

/*
 * Waits for local or remote button press and debounces
 * Returns enum but_type of button pressed
 */
enum but_type but_getaction() {
	while(1) {
		if((~PINC) & PINC_BUTMASK) {
			return but_getlocalbutton();
		}
		if(but_isremotepressed()) {
			return but_getremotebutton();
		}
	}
}

/*
 * Determines whether any buttons, local or remote, have been pressed.
 * Returns non-zero if any are pressed, zero if none pressed.
 */
uint8_t but_ispressed() {
	return but_islocalpressed()||but_isremotepressed();
}

/*
 * Determines whether a local button is pressed
 * Returns the bitfield of pressed buttons, where 1 = pressed
 */
static uint8_t but_islocalpressed() {
	return (~PINC) & PINC_BUTMASK;
}

/*
 * Determines whether a remote button is pressed.
 * Misses presses often since this code doesn't check ICR1.
 */
static uint8_t but_isremotepressed() {
	return (~PINB) & 1<<REM_RX;
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

static char findts(uint16_t time) {
	time /= 200;
	switch(time) {
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'A';
	case 11:
		return 'B';
	case 12:
		return 'C';
	case 13:
		return 'D';
	case 14:
		return 'E';
	case 15:
		return 'F';
	default:
		return 'x';
	}
}

/* Waits for a remote button press, then waits for button to be released.
 * Returns enum but_type of button pressed.
 * Clears TIMER1_CAPT_vect caused by button lifting.
 * Invalid codes return BUT_NONE. This needs to be changed.
 */
static enum but_type but_getremotebutton() {
	uint16_t starttime, endtime, pulselength, data;
	uint8_t command, address;
	
	char msg[17];	// used for debugging
	
	/*
	// look for start pulse 
	do {
		if((~PINB) & 1<<REM_RX) {		// pulse hasn't started
			TCCR1B &= ~(1<<ICES1);		// set capture on falling edge (pulse start)
			TIFR1 = 1<<ICF1;			// clear any capture flag
			while(!(TIFR1 & (1<<ICF1)));// wait for capture
			starttime = ICR1;			// save start timestamp
			TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
			TIFR1 = 1<<ICF1;			// clear capture flag
			while(!(TIFR1 & (1<<ICF1)));// wait for capture
			endtime = ICR1;				// save end timestamp
			TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
			TIFR1 = 1<<ICF1;			// clear capture flag
		} else {						// pulse in progress
			starttime = ICR1;			// get the start time ASAP
			TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
			TIFR1 = 1<<ICF1;			// clear capture flag
			while(!(TIFR1 & (1<<ICF1)));// wait for capture
			endtime = ICR1;				// save end timestamp
			TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
			TIFR1 = 1<<ICF1;			// clear capture flag
		}
		pulselength = endtime - starttime;
		vfd_clear();
		vfd_putchar(findts(pulselength));
	} while(pulselength > 2700 || pulselength < 2100);	// look for 2400 us pulse
	//snprintf(msg, 17, "p=%hu", pulselength);
	//		update_display(msg);
	
	// read in 12-bit data packet
	data = 0;
	for(uint8_t bit = 0; bit < 15; bit++) {
		//_delay_us(450);				// block pulses that are too close together
		
		TCCR1B &= ~(1<<ICES1);		// set capture on falling edge (pulse start)
		TIFR1 = 1<<ICF1;			// clear any capture flag
		while(!(TIFR1 & (1<<ICF1)));// wait for capture
		starttime = ICR1;			// save start timestamp
		TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
		TIFR1 = 1<<ICF1;			// clear capture flag
		while(!(TIFR1 & (1<<ICF1)));// wait for capture
		endtime = ICR1;				// save end timestamp
		TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
		TIFR1 = 1<<ICF1;			// clear capture flag
		pulselength = endtime - starttime;
		
		vfd_putchar(findts(pulselength));
		//snprintf(msg, 17, "p=%hu", pulselength);
		//update_display(msg);
		
		
		if(pulselength < 450) {		// bad pulse
			snprintf(msg, 17, "p=%hu b=%hhu", pulselength, bit);
			update_display(msg);
			//_delay_ms(250);
			return BUT_NONE;
		} else if(pulselength < 900) {// data zero
			//data |= 1<<bit;
		} else if(pulselength < 2400) {// data one
			data |= 1<<bit;
		} else {
			snprintf(msg, 17, "p=%hu b=%hhu", pulselength, bit);
			update_display(msg);
			//_delay_ms(250);
			return BUT_NONE;		// bad pulse
		}
		
		
	} */
	
	// extract command and address from data packet
	//command = data & 0x7f;			// low 7 bits are command
	//address = data>>7;				// high 5 are address
	
	uint16_t t = 0;
	do {
		t++;
		TIFR1 = 1<<ICF1;
		_delay_ms(1);
		if(TIFR1 & (1<<ICF1)) t = 0;
	} while(t < 60);
	
	snprintf(msg, 17, "Code %x");
	update_display(msg);
	
	/*
	if(address != REM_ADDRESS) {	// throw out presses to the wrong address
		return BUT_NONE;
	}
	
	switch(command) {				// translate command to button
	case 0:							// FIXME: correct ENTER and BACK
		return BUT_ENTER;
	case 1:
		return BUT_BACK;
	case 18:
		return BUT_VOLUP;
	case 19:
		return BUT_VOLDN;
	case 16:
		return BUT_LEFT;
	case 17:
		return BUT_RIGHT;
	default:	// for debugging
		snprintf(msg, 17, "CODE: %hhu", command);
		update_display(msg);
		_delay_ms(250);
		return BUT_NONE;
	}*/
	return BUT_NONE;
}

ISR(PCINT1_vect) {
	// only run but_intfunc() if it has been set!
	if(but_intfunc) but_intfunc();
}

ISR(TIMER1_CAPT_vect) {
	// only run but_intfunc() if it has been set!
	if(but_intfunc) but_intfunc();
}