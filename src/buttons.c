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
 * Designed to work with the Sony IR protocol. No guarantees.
 *
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

#define DEBOUNCE_TIME 10	// debounce time, in milliseconds
#define REM_TIMEOUT 40000U	// max time between remote packets, in milliseconds
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
 */
static uint8_t but_isremotepressed() {
	// check for recent unhandled capture
	if((TIFR1 & (1<<ICF1)) && (TCNT1 - ICR1 < REM_TIMEOUT)) {
		TIFR1 = 1<<ICF1;	// clear flag
		return 1;
	} else {
		return 0;
	}
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

/* Waits for a remote button press, then waits for button to be released.
 * Returns enum but_type of button pressed.  Returns BUT_NONE if no packets
 * received within REM_TIMEOUT.
 *
 * Clears TIMER1_CAPT_vect caused by button lifting.
 *
 * Invalid signals may make this hang. This NEEDS TO BE FIXED.
 * Possible idea: monitor TCNT1 in all while loops to create a timeout
 */
static enum but_type but_getremotebutton() {
	uint16_t packet, pulsestart, pulseend, pulselength, listenstart;
	uint8_t good, bit, data, address;
	
	char msg[17];
	
	good = 0;
	while(!good) {
		do {
			if(PINB & 1<<REM_RX) {			// pulse hasn't started
				TCCR1B &= ~(1<<ICES1);		// set capture on falling edge (pulse start)
				TIFR1 = 1<<ICF1;			// clear any capture flag
				listenstart = TCNT1;		// save listen start time
				while((!(TIFR1 & (1<<ICF1)))) {// wait for capture or listen timeout
					if(TCNT1 - listenstart > REM_TIMEOUT) {
						return BUT_NONE;
					}
				}
				pulsestart = ICR1;			// save start timestamp
				TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
				TIFR1 = 1<<ICF1;			// clear capture flag
				while((!(TIFR1 & (1<<ICF1))));// wait for capture
				pulseend = ICR1;				// save end timestamp
				TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
				TIFR1 = 1<<ICF1;			// clear capture flag
			} else {						// pulse in progress
				pulsestart = ICR1;			// get the start time ASAP
				TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
				TIFR1 = 1<<ICF1;			// clear capture flag
				while((!(TIFR1 & (1<<ICF1))));// wait for capture
				pulseend = ICR1;				// save end timestamp
				TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
				TIFR1 = 1<<ICF1;			// clear capture flag
			}
			
			pulselength = pulseend - pulsestart;
			
		} while (pulselength < 2100 || pulselength > 2700);
		
		good = 1;
		packet = bit = 0;
		
		// get 12-bit data packet
		while (good && bit < 12) {
			// look for pulse start
			TCCR1B &= ~(1<<ICES1);		// set capture on falling edge (pulse start)
			TIFR1 = 1<<ICF1;			// clear any capture flag
			listenstart = TCNT1;		// save listen start time
			while((!(TIFR1 & (1<<ICF1)))) {// wait for capture or timeout
				if(TCNT1 - listenstart > REM_TIMEOUT) {
					return BUT_NONE;
				}
			}
			pulsestart = ICR1;			// save start timestamp
			
			// look for pulse end
			TCCR1B |= 1<<ICES1;			// set capture on rising edge (pulse end)
			TIFR1 = 1<<ICF1;			// clear capture flag
			while((!(TIFR1 & (1<<ICF1))));// wait for capture or timeout
			pulseend = ICR1;				// save end timestamp
			TCCR1B &= ~(1<<ICES1);		// set capture back to falling edge
			TIFR1 = 1<<ICF1;			// clear capture flag
			
			pulselength = pulseend - pulsestart;
		
			// interpret pulse
			if(pulselength < 450) {		// too short
				good = packet = 0;		// reset receive
			} else if (pulselength < 750) { // data zero
				bit++;
			} else if (pulselength < 900) { // bad data
				good = packet = 0;		// reset receive
			} else if (pulselength < 1500) {// data one
				packet |= 1<<bit;
				bit++;
			}
		}
		
		// check for correct address
		address = packet >> 7;			// address is in high 5 bits
		if(address != REM_ADDRESS) {	// throw out wrong-address packets
			good = 0;
		}
	}
	
	data = packet & 0x7f;	// get data from lower 7 bits
	
	// wait for button to be lifted
	TCNT1 = 0;
	do {
		if((~PINB) & 1<<REM_RX) TCNT1 = 0;
	} while(TCNT1 < REM_TIMEOUT);
	TIFR1 = 1<<ICF1;			// clear capture flag
	
	// interpret data
	switch(data) {
	case 0x60:
	case 0x65:
		return BUT_ENTER;
	case 0x63:
		return BUT_BACK;
	case 0x12:
		return BUT_VOLINC;
	case 0x13:
		return BUT_VOLDEC;
	case 0x10:
		return BUT_SELDNR;
	case 0x11:
		return BUT_SELUPL;
	case 0x74:
		return BUT_DIRUP;
	case 0x75:
		return BUT_DIRDN;
	case 0x34:
		return BUT_DIRLEFT;
	case 0x33:
		return BUT_DIRRIGHT;
	default:
		snprintf_P(msg, 17, PSTR("E: Remote 0x%02x"), data);
		update_display(msg);
		_delay_ms(250);
		return BUT_NONE;
	}
}

ISR(PCINT1_vect) {
	// only run but_intfunc() if it has been set!
	if(but_intfunc) but_intfunc();
}

ISR(TIMER1_CAPT_vect) {
	// only run but_intfunc() if it has been set!
	if(but_intfunc) but_intfunc();
}
