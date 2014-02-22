/* butenc.c - overhauled human-interface logic 
 * Uses PCINT*/

#if F_CPU != 1000000UL
#error "F_CPU must be 100000 Hz. Other CPU frequencies are not yet supported."
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "pins.h"
#include "buttons.h"

#define BUT_DEBTIME 10		// button debounce time, in milliseconds
#define ENC_DEBTIME 20		// encoder debounce time, in 100 microseconds
#define REM_TIMEOUT 40000U	// max time between remote packets, in microseconds
#define REM_ADDRESS 1		// device address to listen for

static uint8_t gray2num(uint8_t g);	// converts graycode to uint8_t
static void but_check();	// checks for presses and updates but_outstanding
static void enc_check();	// checks for turning and updates but_outstanding
static void rem_check();	// checks for presses and updates but_outstanding

static enum but_type but_outstanding = BUT_NONE;

void butinit() {
	DDRB &= ~(1<<REM_RX);			// make sure IR receiver is input
	PORTB |= 1<<REM_RX;				// enable pullup
	DDRC &= ~(PINC_BUTMASK|PINC_ENCMASK);	// make sure all buttons are inputs
	PORTC |= PINC_BUTMASK|PINC_ENCMASK;		// inputs with pullups, that is
	
	TCCR1A = 0;						// Ensure timer is in 'normal' mode
	TCCR1B |= (1<<ICNC1)|(0<<ICES1)|(1<<CS10);	// Detect falling edge (start of timing), max count rate
	TIMSK1 = 0;
	
	PCICR |= 1<<PCIE1;				// enable pin-change interrupt
	PCMSK1 |= PCI1_MASK;			// and enable each button interrupt
	TIMSK1 |= 1<<ICIE1;				// Enable input compare interrupt for remote receiver
}

enum but_type but_pop() {
	enum but_type os = but_outstanding;
	but_outstanding = BUT_NONE;
	return os;
}

enum but_type but_peek() {
	return but_outstanding;
}

// processes any encoder movement
static void enc_check() {
	static int8_t volblips = 0;	// 4 blips = 1 detent/buttonpress
	static int8_t selblips = 0;
	static uint8_t old_volpos = 4;	// 4 is sentinel value
	static uint8_t old_selpos = 4;
	uint8_t pincstore = PINC;
	uint8_t cur_volpos, cur_selpos, debounce_time;
	
	// on debounce
	debounce_time = ENC_DEBTIME;
	while(debounce_time > 0) {
		if(pincstore != PINC) {	// wait for PINC to settle
			pincstore = PINC;
			cur_volpos = gray2num((((pincstore>>PINC_VOLDN)&1)<<1)|((pincstore>>PINC_VOLUP)&1));
			debounce_time = ENC_DEBTIME;
		}
		_delay_us(100);
		debounce_time--;
	}
	
	// check for volume knob movement
	cur_volpos = gray2num((((pincstore>>PINC_VOLDN)&1)<<1)|((pincstore>>PINC_VOLUP)&1));
	
	if(old_volpos > 3) {		// set old_volpos if run for first time
		old_volpos = cur_volpos;
	} else if(old_volpos != cur_volpos) {	// handle movement
		
		// determine motion (there's got to be a simpler way)
		switch(old_volpos) {
		case 0:
			if(cur_volpos == 1) {
				volblips++;
			} else if(cur_volpos == 3) {
				volblips--;
			}
			break;
		case 1:
			if(cur_volpos == 2) {
				volblips++;
			} else if(cur_volpos == 0) {
				volblips--;
			}
			break;
		case 2:
			if(cur_volpos == 3) {
				volblips++;
			} else if(cur_volpos == 1) {
				volblips--;
			}
			break;
		case 3:
			if(cur_volpos == 0) {
				volblips++;
			} else if(cur_volpos == 2) {
				volblips--;
			}
			break;
		}
		
		if(volblips >= 4) {
			but_outstanding = BUT_VOLINC;
			volblips = 0;
		} else if(volblips <= -4) {
			but_outstanding = BUT_VOLDEC;
			volblips = 0;
		}
		
		if(cur_volpos == 0) volblips = 0;	// keep volblips aligned with detent
		
		old_volpos = cur_volpos;
	}
	
	// check for selector knob movement
	cur_selpos = gray2num((((pincstore>>PINC_RIGHT)&1)<<1)|((pincstore>>PINC_LEFT)&1));
	
	if(old_selpos > 3) {		// set old_volpos if run for first time
		old_selpos = cur_selpos;
	} else if(old_selpos != cur_selpos) {	// handle movement
		
		// determine motion (there's got to be a simpler way)
		switch(old_selpos) {
		case 0:
			if(cur_selpos == 1) {
				selblips++;
			} else if(cur_selpos == 3) {
				selblips--;
			}
			break;
		case 1:
			if(cur_selpos == 2) {
				selblips++;
			} else if(cur_selpos == 0) {
				selblips--;
			}
			break;
		case 2:
			if(cur_selpos == 3) {
				selblips++;
			} else if(cur_selpos == 1) {
				selblips--;
			}
			break;
		case 3:
			if(cur_selpos == 0) {
				selblips++;
			} else if(cur_selpos == 2) {
				selblips--;
			}
			break;
		}
		
		if(selblips >= 4) {
			but_outstanding = BUT_SELUPL;
			volblips = 0;
		} else if(selblips <= -4) {
			but_outstanding = BUT_SELDNR;
			selblips = 0;
		}
		
		if(cur_selpos == 0) selblips = 0;	// keep selblips aligned with detent
		
		old_selpos = cur_selpos;
	}
}

// processes any pressed local buttons
static void but_check() {
	uint8_t pressedbyte = (~PINC) & PINC_BUTMASK;
	
	if(pressedbyte) {
		for(uint8_t timeleft = BUT_DEBTIME; timeleft > 0; timeleft--) {
			if((PINC & PINC_BUTMASK) != PINC_BUTMASK) timeleft = BUT_DEBTIME;
			_delay_ms(1);
		}
		
		but_outstanding = pressedbyte;
	}
}

/* Waits for a remote button press, then waits for button to be released.
 * Returns enum but_type of button pressed.  Returns BUT_NONE if no packets
 * received within REM_TIMEOUT.
 *
 * Clears TIMER1_CAPT_vect caused by button lifting.
 *
 * If REM_TIMEOUT passes, sets outstanding to BUT_NONE
 */
static void rem_check() {
	uint16_t packet, pulsestart, pulseend, pulselength, listenstart;
	uint8_t good, bit, data, address;
	
	good = 0;
	while(!good) {
		do {
			if(PINB & 1<<REM_RX) {			// pulse hasn't started
				TCCR1B &= ~(1<<ICES1);		// set capture on falling edge (pulse start)
				TIFR1 = 1<<ICF1;			// clear any capture flag
				listenstart = TCNT1;		// save listen start time
				while((!(TIFR1 & (1<<ICF1)))) {// wait for capture or listen timeout
					if(TCNT1 - listenstart > REM_TIMEOUT) {
						return;				// abort listen in case of timeout
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
				if(TCNT1 - listenstart > 900) {	
					return;				// abort listen in case of timeout
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
		but_outstanding = BUT_ENTER;
		break;
	case 0x63:
		but_outstanding = BUT_BACK;
		break;
	case 0x12:
		but_outstanding = BUT_VOLINC;
		break;
	case 0x13:
		but_outstanding = BUT_VOLDEC;
		break;
	case 0x10:
		but_outstanding = BUT_SELDNR;
		break;
	case 0x11:
		but_outstanding = BUT_SELUPL;
		break;
	case 0x74:
		but_outstanding = BUT_DIRUP;
		break;
	case 0x75:
		but_outstanding = BUT_DIRDN;
		break;
	case 0x34:
		but_outstanding = BUT_DIRLEFT;
		break;
	case 0x33:
		but_outstanding = BUT_DIRRIGHT;
		break;
	default:
		//but_outstanding = BUT_NONE;
		break;
	}
}

// called when a rotary encoder or button state changes
ISR(PCINT1_vect) {
	but_check();
	enc_check();
	PCIFR = 1<<PCIF1;	// clear PCINT1 caused by switch motion
}

// called when an IR pulse is received
ISR(TIMER1_CAPT_vect) {
	rem_check();
}

static uint8_t gray2num(uint8_t g) {
	switch(g) {
	case 3:
		return 0;
		break;
	case 2:
		return 1;	// 0g10 == 0b11
		break;
	case 0:
		return 2;
		break;
	case 1:
	default:
		return 3;
		break;
	}
}