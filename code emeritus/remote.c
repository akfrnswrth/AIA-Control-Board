/*
 * remote.c
 * Author: Ali Kocaturk 1/4/14
 * 
 * sort of compatible with Sony remotes
 * probably the buggiest/sketchiest code in here; none of the timing makes sense
 * NONE OF IT
 */
 
 #include <avr/io.h>
 #include <util/delay.h>
 #include "pins.h"
 #define _REM_HALF 450
 
 void reminit() {
 	DDRD &= ~(1<<REM_RX);	// input
 	PORTD |= 1<<REM_RX;		// pullup
 }
 
int8_t rem_getpress() {
 	uint8_t halves;			// timebase for SIRC is 600us
 	uint8_t code;
 	uint8_t bit;
 	
 	// wait for correct 2100-2700 us start pulse
 	do {
 		halves = 0;
	 	while((PIND & (1<<REM_RX)) == 0) {
 			_delay_us(_REM_HALF);
 			halves++;
 		}
 	} while (halves < 6 || halves > 9);
 	
 	// wait for pulse to end
 	halves = 0;
 	while(PIND & (1<<REM_RX)) {
 		_delay_us(_REM_HALF);
 		halves++;
 	}
 	if(halves > 3) return -1;	// invalid code
 	
 	code = 0;
 	for(bit = 0; bit < 7; bit++) {
 		halves = 0;
 		while(PIND & (1<<REM_RX));
 		while(PIND & (1<<REM_RX)) {
	 		_delay_us(_REM_HALF);
	 		halves++;
	 	}
	 	if(halves > 3) return -1;	// invalid code
		
		halves = 0;
 		while((PIND & (1<<REM_RX)) == 0) {
 			_delay_us(_REM_HALF);
 			if(bit>1) _delay_us(_REM_HALF/4);
 			halves++;
 		}
 		if(halves > 2) {
 			code |= 1<<bit;
 		}
 	}
 	
 	// trusty debounce code works for repeated remote codes, too
 	for(int8_t timeleft = 30; timeleft > 0; timeleft--) {
		if((PIND & (1<<REM_RX)) == 0) timeleft = 50;
		_delay_ms(1);
	}
	
	return code;
}
