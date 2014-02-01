/*
 * ui.c - the UI (menus & button interpretation) for AIA Control Board
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
#include "buttons.h"
#include "lang.h"
#include "ui.h"

#define UI_HOLD_TIME 30		// time to hold volume value on display, in centiseconds

static volatile uint8_t idle_timeout = UI_HOLD_TIME;	// centisecond idle timeout counter

static void ui_idle();
static void ui_showinput();
static void ui_buttonISR();
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
	but_setint(ui_buttonISR);
	
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
}

/*
 * Root menu
 */
static void ui_rootmenu() {
	uint8_t choice = 0;
	enum but_type pressed;
	
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
		
		pressed = but_getaction();
		
		switch(pressed) {
		case (BUT_LEFT):
			if(choice == 0) {
				choice = 3;
			} else {
				choice--;
			}
			break;
		case (BUT_RIGHT):
			choice++;
			if(choice > 3) choice = 0;
			break;
		case (BUT_ENTER):
			if(choice == 0) {
				ui_tonemenu();
			} else if(choice == 2) {
				ui_namemenu();
			} else if(choice == 3) {
				ui_brightnessmenu();
			}
			break;
		case (BUT_VOLUP):
			if(choice == 1) {
				pre_increasespkbehavior();
			}
			break;
		case (BUT_VOLDN):
			if(choice == 1) {
				pre_decreasespkbehavior();
			}
		default:	// catch other enum values
			break;
		}

	} while(pressed != (BUT_BACK));	// menu stays until user exits
	
}

/*
 * Tone behavior & control menu
 */
static void ui_tonemenu() {
	uint8_t choice = 0;
	enum but_type pressed;

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
		
		pressed = but_getaction();
		
		switch(pressed) {
		case (BUT_LEFT):
			if(choice == 0) {
				choice = 2;
			} else {
				choice--;
			}
			break;
		case (BUT_RIGHT):
			choice++;
			if(choice > 2) choice = 0;
			break;
		case (BUT_VOLUP):
			if(choice == 0) {
				pre_increasetonebehavior();
			} else if(choice == 1) {
				pre_increasebass();
			} else if(choice == 2) {
				pre_increasetreb();
			}
			break;
		case (BUT_VOLDN):
			if(choice == 0) {
				pre_decreasetonebehavior();
			} else if(choice == 1) {
				pre_decreasebass();
			} else if(choice == 2) {
				pre_decreasetreb();
			}
			break;
		default:	// catch other enum values
			break;
		}
		
	} while(pressed != (BUT_BACK));
}

/*
 * Name edit choice menu
 */
static void ui_namemenu() {
	uint8_t choice = 0;
	enum but_type pressed;
	char msg[17];
	char name[17];
	
	do {
		
		name_get(name, choice);
		name_getprefix(msg, choice);
		strlcat(msg, name, 17);
		update_display(msg);
		
		pressed = but_getaction();
		
		switch(pressed) {
		case (BUT_LEFT):
			if(choice == 0) {
				choice = 7;
			} else {
				choice--;
			}
			break;
		case (BUT_RIGHT):
			choice++;
			if(choice > 7) choice = 0;
			break;
		case (BUT_ENTER):
			ui_nameedit(choice);
			break;
		default:	// catch other enum values
			break;
		}
		
	} while(pressed != (BUT_BACK));
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
	enum but_type pressed;              // button state storage
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
		} while(!but_ispressed());
		pressed = but_getaction();	// debounce
		
		switch(pressed) {
		case (BUT_LEFT):
			if(edit_pos > 0) {
				name_cursor[edit_pos] = name[edit_pos];	// clear old cursor
				edit_pos--;
			}
			name_cursor[edit_pos] = 0x7f;
			break;
		case (BUT_RIGHT):
			name_cursor[edit_pos] = name[edit_pos];	// clear old cursor
			edit_pos++;
			if(edit_pos > max_edit_pos) {
				edit_pos = max_edit_pos;
			}
			name_cursor[edit_pos] = 0x7f;
			break;
		case (BUT_VOLUP):
			name[edit_pos]++;

			if(name[edit_pos] == '!') name[edit_pos] = 'a';
			else if(name[edit_pos] >= '{') name[edit_pos] = 'A';
			else if(name[edit_pos] == '[') name[edit_pos] = '0';
			else if(name[edit_pos] == ':') name[edit_pos] = '\'';
			else if(name[edit_pos] == '*') name[edit_pos] = ' ';

			name_cursor[edit_pos] = name[edit_pos];

			break;
		case (BUT_VOLDN):
			name[edit_pos]--;
			
			if(name[edit_pos] < ' ') name[edit_pos] = ')';
			else if(name[edit_pos] == '&') name[edit_pos] = '9';
			else if(name[edit_pos] == '/') name[edit_pos] = 'Z';
			else if(name[edit_pos] == '`') name[edit_pos] = ' ';
			else if(name[edit_pos] == '@') name[edit_pos] = 'z';
			
			name_cursor[edit_pos] = name[edit_pos];
			
			break;
		default:	// catch other possible enum values
			break;
		}
	} while(pressed != BUT_BACK);
	
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
	enum but_type pressed;
	
	do {
		switch(choice) {
		case 0:
			ui_showactivebrightness();
			break;
		case 1:
			ui_showidlebrightness();
			break;
		}
		
		pressed = but_getaction();
		
		switch(pressed) {
		case (BUT_LEFT):
		case (BUT_RIGHT):
			if(choice == 0) {
				choice = 1;
			} else {
				choice = 0;
			}
			break;
		case (BUT_VOLUP):
			if(choice == 0) {
				vfd_increaseactivebrightness();
			} else if(choice == 1) {
				vfd_increaseidlebrightness();
			}
			break;
		case (BUT_VOLDN):
			if(choice == 0) {
				vfd_decreaseactivebrightness();
			} else if(choice == 1) {
				vfd_decreaseidlebrightness();
			}
			break;
		default:	// catch other enum values
			break;
		}
		
	} while(pressed != (BUT_BACK));
	
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
 * Triggered when a button is pressed
 * Volume and input adjust, root menu entry point
 */
static void ui_buttonISR() { 
	enum but_type pressed;
	char msg[17];	// buffer to reduce flicker while adjusting volume
	
	pressed = but_getaction();
	
	vfd_activebrightness();	// set VFD to active brightness
	
	switch(pressed) {
	case (BUT_VOLUP):
		snprintf_P(msg, 17, LANG_VOLUME, pre_increasevol());
		update_display(msg);
		break;
	case (BUT_VOLDN):
		snprintf_P(msg, 17, LANG_VOLUME, pre_decreasevol());
		update_display(msg);
		break;
	case (BUT_LEFT):
		pre_previnput();
		ui_showinput();
		break;
	case (BUT_RIGHT):
		pre_nextinput();
		ui_showinput();
		break;
	case (BUT_ENTER):	// ooh menu time
		ui_rootmenu();		// enter the root menu
	default:
		ui_showinput();		// go back to regular display after exiting root or hitting back
	}
	idle_timeout = UI_HOLD_TIME;
}
