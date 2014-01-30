/*
 * ui.c
 * the UI (menus & button interpretation) for AIA Control Board
 */

#include <avr/io.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "vfd.h"
#include "inputnames.h"
#include "preamp.h"
#include "pins.h"
#include "lang.h"
#include "ui.h"

#define DEBOUNCE_TIME 40	// debounce time, in milliseconds
#define UI_HOLD_TIME 30		// time to hold volume value on display, in centiseconds

static volatile uint8_t idle_timeout = UI_HOLD_TIME;	// centisecond idle timeout counter

static void ui_idle();
static char ui_getbuttons();
static void ui_showinput();
static void ui_rootmenu();
static void ui_tonemenu();
static void ui_namemenu();
static void ui_nameedit(uint8_t);
static void ui_brightnessmenu();

static void ui_showspeaker();
static void ui_showactivebrightness();
static void ui_showidlebrightness();
static void ui_showtoneactive();
static void ui_showtonebass();
static void ui_showtonetreb();

/*
 * Note that this does NOT set global interrupts; that is done
 * when uiloop is called.
 */
void uiinit() {
	DDRC &= ~(BUT_MASK);			// make sure all buttons are inputs
	PORTC |= BUT_MASK;				// inputs with pullups, that is
	
	PCICR |= 1<<PCIE1;				// enable pin-change interrupt
	PCMSK1 |= PC_MASK;				// and enable each button interrupt
	
	set_sleep_mode(SLEEP_MODE_IDLE);// set IDLE as the sleep mode
	
	center_display_P(LANG_SPLASH);	// show splash message
}

/*
 * handles any delays left by interrupts, then puts CPU to sleep
 */
void uiloop() {
	cli();							// block interrupts from messing with idle_timeout access
	while(idle_timeout > 0) {		// delay ui_idle until timeout expires to avoid excessive EEPROM writes
		idle_timeout--;
		sei();						// make sure interrupts work while waiting
		_delay_ms(100);
		cli();
	}
	ui_idle();						// run idle tasks (show input, save settings to EEPROM)
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
}

/*
 * shows the currently selected input
 */
static void ui_showinput() {
	char msg[17];
	name_get(msg, pre_getcurrentinput());
	center_display(msg);
}

/*
 * Idle tasks
 * (show input and then save settings)
 */
static void ui_idle() {
	ui_showinput();			// show current input while idle
	pre_save();				// save volume, tone, input settings
	vfd_idlebrightness();	// set display to idle brightness
	//vfd_save();			// TODO: move this code to brightness menu
}

/*
 * Saves and then debounces button input
 * Returns char with pressed buttons logic 1
 * Clears PCINT1 caused by button lifting
 * TODO: move to a separate file to handle encoders
 */
static char ui_getbuttons() {
	char pressed = (~PINC) & BUT_MASK;
	
	while(!pressed) pressed = (~PINC) & BUT_MASK;	// fixes issues with turn-on bounce
	
	// wait for continuous DEBOUNCE_TIME ms of all buttons open
	for(uint8_t timeleft = DEBOUNCE_TIME; timeleft > 0; timeleft--) {
		if((PINC & BUT_MASK) != BUT_MASK) timeleft = DEBOUNCE_TIME;
		_delay_ms(1);
	}
	PCIFR = 1<<PCIF1;	// clear PCINT1 caused by switch lifting
	
	return pressed;
}

/*
 * Root menu
 */
static void ui_rootmenu() {
	uint8_t choice = 0;
	char pressed;
	
	// show user a list of choices and allow selection
	do {
		switch(choice) {
		case 0:
			update_display_P(LANG_TC);		// tone control
			break;
		case 1:
			ui_showspeaker();
			break;
		case 2:
			update_display_P(LANG_INNAMES);	// input name setting
			break;
		case 3:
			update_display_P(LANG_BRIGHTNESS);
			break;
		}
		
		pressed = ui_getbuttons();
		
		switch(pressed) {
		case (1<<BUT_LEFT):
			if(choice == 0) {
				choice = 3;
			} else {
				choice--;
			}
			break;
		case (1<<BUT_RIGHT):
			choice++;
			if(choice > 3) choice = 0;
			break;
		case (1<<BUT_ENTER):
			if(choice == 0) {
				ui_tonemenu();
			} else if(choice == 2) {
				ui_namemenu();
			} else if(choice == 3) {
				ui_brightnessmenu();
			}
			break;
		case (1<<BUT_VOLUP):
			if(choice == 1) {
				pre_increasespkbehavior();
			}
			break;
		case (1<<BUT_VOLDN):
			if(choice == 1) {
				pre_decreasespkbehavior();
			}
		}

	} while(pressed != (1<<BUT_BACK));	// menu stays until user exits
	
}

/*
 * Tone behavior & control menu
 */
static void ui_tonemenu() {
	uint8_t choice = 0;
	char pressed;

	do {
		switch(choice) {
		case 0:
			ui_showtoneactive();
			break;
		case 1:
			ui_showtonebass();
			break;
		case 2:
			ui_showtonetreb();
			break;
		}
		
		pressed = ui_getbuttons();
		
		switch(pressed) {
		case (1<<BUT_LEFT):
			if(choice == 0) {
				choice = 2;
			} else {
				choice--;
			}
			break;
		case (1<<BUT_RIGHT):
			choice++;
			if(choice > 2) choice = 0;
			break;
		case (1<<BUT_VOLUP):
			if(choice == 0) {
				pre_increasetonebehavior();
			} else if(choice == 1) {
				pre_increasebass();
			} else if(choice == 2) {
				pre_increasetreb();
			}
			break;
		case (1<<BUT_VOLDN):
			if(choice == 0) {
				pre_decreasetonebehavior();
			} else if(choice == 1) {
				pre_decreasebass();
			} else if(choice == 2) {
				pre_decreasetreb();
			}
			break;
		}
		
	} while(pressed != (1<<BUT_BACK));
}

/*
 * Name edit choice menu
 */
static void ui_namemenu() {
	uint8_t choice = 0;
	char pressed;
	char msg[17];
	char name[17];
	
	do {
		
		name_get(name, choice);
		name_getprefix(msg, choice);
		strlcat(msg, name, 17);
		update_display(msg);
		
		pressed = ui_getbuttons();
		
		switch(pressed) {
		case (1<<BUT_LEFT):
			if(choice == 0) {
				choice = 7;
			} else {
				choice--;
			}
			break;
		case (1<<BUT_RIGHT):
			choice++;
			if(choice > 7) choice = 0;
			break;
		case (1<<BUT_ENTER):
			ui_nameedit(choice);
			break;
		}
		
	} while(pressed != (1<<BUT_BACK));
}

/*
 * edits the input name
 * NOTE: this is likely to be where the most RAM usage occurs
 *		 due to the three buffers
 */
static void ui_nameedit(uint8_t n_input) {
	uint8_t edit_pos;
	uint8_t max_edit_pos;
	uint8_t lastnonspace;
	char name[17];						// the name being edited
	char name_cursor[17];				// the name with blinking cursor overlay
	char msg[17];						// what is shown on the VFD
	char pressed;						// button state storage
	uint8_t cursortime = 0;				// used to time cursor blinks
	
	name_get(name, n_input);			// load the old input name
	name_getprefix(msg, n_input);		// load the permanent input name/prefix
	max_edit_pos = 15 - strlen(msg);	// and figure out how much space is left
	
	// pad name and name_cursor with spaces
	edit_pos = 0;
	while(name[edit_pos] != 0 && edit_pos <= max_edit_pos) {
		name_cursor[edit_pos] = name[edit_pos];
		edit_pos++;
	}
	while(edit_pos <= max_edit_pos) {
		name_cursor[edit_pos] = name[edit_pos] = ' ';
		edit_pos++;
	}
	name_cursor[max_edit_pos+1] = name[max_edit_pos+1] = 0;
	edit_pos = 0;
	
	do {
		// blinking cursor
		do {
			_delay_ms(5);
			cursortime++;
			
			if(cursortime > 100) {
				name_cursor[edit_pos] = name[edit_pos];
				cursortime = 0;
			} else if(cursortime == 50) {
				name_cursor[edit_pos] = 0x7f;	// fully-lit block character
			}
			
			name_getprefix(msg, n_input);
			strcat(msg, name_cursor);
			update_display(msg);
			
			pressed = (~PINC) & BUT_MASK;
		} while(!pressed);
		pressed = ui_getbuttons();	// debounce
		
		switch(pressed) {
		case (1<<BUT_LEFT):
			if(edit_pos > 0) {
				name_cursor[edit_pos] = name[edit_pos];	// clear old cursor
				edit_pos--;
			}
			name_cursor[edit_pos] = 0x7f;
			break;
		case (1<<BUT_RIGHT):
			name_cursor[edit_pos] = name[edit_pos];	// clear old cursor
			edit_pos++;
			if(edit_pos > max_edit_pos) {
				edit_pos = max_edit_pos;
			}
			name_cursor[edit_pos] = 0x7f;
			break;
		case (1<<BUT_VOLUP):
			name[edit_pos]++;

			if(name[edit_pos] == '!') name[edit_pos] = 'a';
			else if(name[edit_pos] >= '{') name[edit_pos] = 'A';
			else if(name[edit_pos] == '[') name[edit_pos] = '0';
			else if(name[edit_pos] == ':') name[edit_pos] = '\'';
			else if(name[edit_pos] == '*') name[edit_pos] = ' ';

			name_cursor[edit_pos] = name[edit_pos];

			break;
		case (1<<BUT_VOLDN):
			name[edit_pos]--;
			
			if(name[edit_pos] < ' ') name[edit_pos] = ')';
			else if(name[edit_pos] == '&') name[edit_pos] = '9';
			else if(name[edit_pos] == '/') name[edit_pos] = 'Z';
			else if(name[edit_pos] == '`') name[edit_pos] = ' ';
			else if(name[edit_pos] == '@') name[edit_pos] = 'z';
			
			name_cursor[edit_pos] = name[edit_pos];
			
			break;
		}
	} while(pressed != 1<<BUT_BACK);
	// TODO: Filter out trailing spaces and save name
	
	// filter out trailing spaces
	lastnonspace = 0;
	for(edit_pos = 0; edit_pos <= max_edit_pos; edit_pos++) {
		if(name[edit_pos] != ' ') lastnonspace = edit_pos;
	}
	name[lastnonspace+1] = 0;	// truncate after last non-space
	
	name_put(name, n_input);	// save name
}

/*
 * Brightness adjustment menu
 */
static void ui_brightnessmenu() {
	uint8_t choice = 0;
	char pressed;
	
	do {
		switch(choice) {
		case 0:
			ui_showactivebrightness();
			break;
		case 1:
			ui_showidlebrightness();
			break;
		}
		
		pressed = ui_getbuttons();
		
		switch(pressed) {
		case (1<<BUT_LEFT):
		case (1<<BUT_RIGHT):
			if(choice == 0) {
				choice = 1;
			} else {
				choice = 0;
			}
			break;
		case (1<<BUT_VOLUP):
			if(choice == 0) {
				vfd_increaseactivebrightness();
			} else if(choice == 1) {
				vfd_increaseidlebrightness();
			}
			break;
		case (1<<BUT_VOLDN):
			if(choice == 0) {
				vfd_decreaseactivebrightness();
			} else if(choice == 1) {
				vfd_decreaseidlebrightness();
			}
			break;
		}
		
	} while(pressed != (1<<BUT_BACK));
	
	vfd_save();		// save brightness
	
}

/*
 * shows the current tone control behavior setting
 */
static void ui_showtoneactive() {
	enum pre_tonebehavior cbhr = pre_gettonebehavior();
	PGM_P msg_P;
	
	switch(cbhr) {
	case TONE_SPKONLY:
		msg_P = LANG_TC_SPKONLY;
		break;
	case TONE_ALWAYS:
		msg_P = LANG_TC_ALWAYS;
		break;
	case TONE_NEVER:
		msg_P = LANG_TC_NEVER;
		break;
	default:		// if this happens, the program is probably corrupt
		msg_P = ERROR_CANTHAPPEN;
		break;
	}
	update_display_P(msg_P);
}

/*
 * shows the current bass setting
 */
static void ui_showtonebass() {
	char msg[17];
	
	snprintf_P(msg, 17, LANG_TC_BASS, pre_getbass());
	update_display(msg);
}

/*
 * shows the current treble setting
 */
static void ui_showtonetreb() {
	char msg[17];
	
	snprintf_P(msg, 17, LANG_TC_TREBLE, pre_gettreb());
	update_display(msg);
}

/*
 * shows the current speaker setting
 */
static void ui_showspeaker() {
	enum pre_spkbehavior cbhr = pre_getspkbehavior();
	PGM_P msg_P;
	
	switch(cbhr) {
	case SPK_AUTO:
		msg_P = LANG_SPK_AUTO;
		break;
	case SPK_ON:
		msg_P = LANG_SPK_ON;
		break;
	case SPK_OFF:
		msg_P = LANG_SPK_OFF;
		break;
	default:		// seriously this shouldn't happen ever
		msg_P = ERROR_CANTHAPPEN;
		break;
	}
	update_display_P(msg_P);
}

/*
 * shows the current active brightness setting
 */
static void ui_showactivebrightness() {
	char msg[17];
	snprintf_P(msg, 17, LANG_ACTIVEBRIGHTNESS, vfd_getactivebrightness());
	update_display(msg);
}

/*
 * shows the current idle brightness setting
 */
static void ui_showidlebrightness() {
	char msg[17];
	snprintf_P(msg, 17, LANG_IDLEBRIGHTNESS, vfd_getidlebrightness());
	update_display(msg);
}

/*
 * Triggered when logic level changes on a button pin
 * Volume and input adjust, root menu entry point
 */
ISR(PCINT1_vect) { 
	char pressed;
	char msg[17];	// buffer to reduce flicker while adjusting volume
	
	pressed = ui_getbuttons();
	
	vfd_activebrightness();	// set VFD to active brightness
	
	switch(pressed) {
	case (1<<BUT_VOLUP):
		snprintf_P(msg, 17, LANG_VOLUME, pre_increasevol());
		update_display(msg);
		break;
	case (1<<BUT_VOLDN):
		snprintf_P(msg, 17, LANG_VOLUME, pre_decreasevol());
		update_display(msg);
		break;
	case (1<<BUT_LEFT):
		pre_previnput();
		ui_showinput();
		break;
	case (1<<BUT_RIGHT):
		pre_nextinput();
		ui_showinput();
		break;
	case (1<<BUT_ENTER):	// ooh menu time
		ui_rootmenu();		// enter the root menu
	case (1<<BUT_BACK):
		ui_showinput();		// go back to regular display after exiting root or hitting back
	}
	idle_timeout = UI_HOLD_TIME;
}
