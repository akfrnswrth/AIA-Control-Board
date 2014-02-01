/* 
 * inputnames.c - handling of custom input names
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
 * Handles custom input names max 16 chars long
 */
 
#include <avr/io.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "vfd.h"
#include "lang.h"
#include "pins.h"
#include "inputnames.h"

static char ee_name0[17] EEMEM = "Aux";
static char ee_name1[17] EEMEM = "Line 1";
static char ee_name2[17] EEMEM = "Line 2";
static char ee_name3[17] EEMEM = "Line 3";
static char ee_name4[17] EEMEM = "Line 4";
static char ee_name5[17] EEMEM = "Line 5";
static char ee_name6[17] EEMEM = "Line 6";
static char ee_name7[17] EEMEM = "Line 7";

static char * ee_name_findaddr(uint8_t name_num);

void name_get(char * buf, uint8_t name_num) {
	//DDRB |= 1<<EE_LED;		// make EEPROM access LED output
	//PORTB |= 1<<EE_LED;		// turn on LED
	eeprom_read_block(buf, ee_name_findaddr(name_num), 17);
	//PORTB &= ~(1<<EE_LED);	// turn off LED
}

void name_getprefix(char * buf, uint8_t name_num) {
	PGM_P prefix_P;
	
	switch(name_num) {
	case 0:
		prefix_P = LANG_IN0;
		break;
	case 1:
		prefix_P = LANG_IN1;
		break;
	case 2:
		prefix_P = LANG_IN2;
		break;
	case 3:
		prefix_P = LANG_IN3;
		break;
	case 4:
		prefix_P = LANG_IN4;
		break;
	case 5:
		prefix_P = LANG_IN5;
		break;
	case 6:
		prefix_P = LANG_IN6;
		break;
	case 7:
		prefix_P = LANG_IN7;
		break;
	default:		// if this happens, the program is probably corrupt
		prefix_P = ERROR_CANTHAPPEN;
		break;
	}
	strlcpy_P(buf, prefix_P, 17);
}

void name_put(const char * newname, uint8_t name_num) {
	DDRB |= 1<<EE_LED;		// make EEPROM access LED output
	PORTB |= 1<<EE_LED;		// turn on LED
	eeprom_update_block(newname, ee_name_findaddr(name_num), 17);
	PORTB &= ~(1<<EE_LED);	// turn off LED
}

static char * ee_name_findaddr(uint8_t name_num) {
	switch(name_num) {
	case 0:
		return ee_name0;
	case 1:
		return ee_name1;
	case 2:
		return ee_name2;
	case 3:
		return ee_name3;
	case 4:
		return ee_name4;
	case 5:
		return ee_name5;
	case 6:
		return ee_name6;
	case 7:
		return ee_name7;
	default:
		return NULL;
	}
}
