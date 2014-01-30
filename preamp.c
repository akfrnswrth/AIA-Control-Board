/* preamp.c - Preamp functions
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
 * 
 * handles setting and loading the preamp functions
 * (volume, treble, bass, input select)
 * and some amp functions
 * (speaker/headphone select)
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "pins.h"
#include "preamp.h"

#define PRE_NINPUTS 8
#define PRE_MINTONE (-12)
#define PRE_MAXTONE 12
#define PRE_MAXVOL 127

// Non-volatile settings
static uint8_t ee_volume EEMEM = 0;
static uint8_t ee_inselect EEMEM = 0;
static enum pre_tonebehavior ee_tonebehavior EEMEM = TONE_SPKONLY;
static int8_t ee_bass EEMEM = 0;
static int8_t ee_treb EEMEM = 0;
static enum pre_spkbehavior ee_spkbehavior EEMEM = SPK_AUTO;

// Volatile copies
static uint8_t ram_volume = 0;
static uint8_t ram_inselect = 0;
static enum pre_tonebehavior ram_tonebehavior = TONE_SPKONLY;
static enum pre_spkbehavior ram_spkbehavior = SPK_AUTO;
static int8_t ram_bass = 0;
static int8_t ram_treb = 0;

static void pre_load();

void preinit() {
	DDRB |= 1<<EE_LED;		// make EEPROM access LED output
	PORTB &= ~(1<<EE_LED);	// turn off LED
	
	// TODO: HW initialization here

	pre_load();
}

/*
 * Loads all preamp settings from EEPROM
 */
static void pre_load() {
	//PORTB |= 1<<EE_LED;		// turn on EEPROM access LED
	ram_volume = eeprom_read_byte(&ee_volume);
	ram_inselect = eeprom_read_byte(&ee_inselect);
	eeprom_read_block(&ram_bass, &ee_bass, sizeof(ee_bass));
	eeprom_read_block(&ram_treb, &ee_treb, sizeof(ee_treb));
	eeprom_read_block(&ram_tonebehavior, &ee_tonebehavior, sizeof(ee_tonebehavior));
	eeprom_read_block(&ram_spkbehavior, &ee_spkbehavior, sizeof(ee_spkbehavior));
	//PORTB &= ~(1<<EE_LED);	// turn off EEPROM access LED
}

/*
 * Saves all preamp settings to EEPROM
 */
void pre_save() {
	PORTB |= 1<<EE_LED;		// turn on EEPROM access LED
	eeprom_update_byte(&ee_inselect, ram_inselect);
	eeprom_update_byte(&ee_volume, ram_volume);
	eeprom_update_block(&ram_bass, &ee_bass, sizeof(ee_bass));
	eeprom_update_block(&ram_treb, &ee_treb, sizeof(ee_treb));
	eeprom_update_block(&ram_tonebehavior, &ee_tonebehavior, sizeof(ee_tonebehavior));
	eeprom_update_block(&ram_spkbehavior, &ee_spkbehavior, sizeof(ee_spkbehavior));
	PORTB &= ~(1<<EE_LED);	// turn off EEPROM access LED
}

/*
 * Gets the tone behavior
 */
enum pre_tonebehavior pre_gettonebehavior() {
	return ram_tonebehavior;
}

/*
 * Increases the tone behavior setting
 * Returns new tone behavior
 */
enum pre_tonebehavior pre_increasetonebehavior() {
	switch(ram_tonebehavior) {
	case TONE_SPKONLY:
		ram_tonebehavior = TONE_NEVER;
		break;
	case TONE_ALWAYS:
		ram_tonebehavior = TONE_SPKONLY;
		break;
	case TONE_NEVER:
		ram_tonebehavior = TONE_ALWAYS;
		break;
	}
	return ram_tonebehavior;
}

/*
 * Decreases the tone behavior setting
 * Returns new tone behavior
 */
enum pre_tonebehavior pre_decreasetonebehavior() {
	switch(ram_tonebehavior) {
	case TONE_SPKONLY:
		ram_tonebehavior = TONE_ALWAYS;
		break;
	case TONE_ALWAYS:
		ram_tonebehavior = TONE_NEVER;
		break;
	case TONE_NEVER:
		ram_tonebehavior = TONE_SPKONLY;
		break;
	}
	return ram_tonebehavior;
}

/*
 * gets the bass setting
 */
int8_t pre_getbass() {
	return ram_bass;
}

/*
 * increases the bass setting, if possible
 * returns the new bass setting
 */
int8_t pre_increasebass() {
	ram_bass++;
	if(ram_bass > PRE_MAXTONE) ram_bass = PRE_MAXTONE;
	// TODO: HW control
	return ram_bass;
}

/*
 * decreases the bass setting, if possible
 * returns the new bass setting
 */
int8_t pre_decreasebass() {
	ram_bass--;
	if(ram_bass < PRE_MINTONE) ram_bass = PRE_MINTONE;
	// TODO: HW control
	return ram_bass;
}

/*
 * gets the treble setting
 */
int8_t pre_gettreb() {
	return ram_treb;
}

/*
 * increases the treble setting, if possible
 * returns the new bass setting
 */
int8_t pre_increasetreb() {
	ram_treb++;
	if(ram_treb > PRE_MAXTONE) ram_treb = PRE_MAXTONE;
	// TODO: HW control
	return ram_treb;
}

/*
 * decreases the treble setting, if possible
 * returns the new treble setting
 */
int8_t pre_decreasetreb() {
	ram_treb--;
	if(ram_treb < PRE_MINTONE) ram_treb = PRE_MINTONE;
	// TODO: HW control
	return ram_treb;
}

/*
 * Increases the speaker behavior setting
 * Returns new speaker behavior
 */
enum pre_spkbehavior pre_increasespkbehavior() {
	switch(ram_spkbehavior) {
	case SPK_AUTO:
		ram_spkbehavior = SPK_OFF;
		break;
	case SPK_ON:
		ram_spkbehavior = SPK_AUTO;
		break;
	case SPK_OFF:
		ram_spkbehavior = SPK_ON;
		break;
	}
	return ram_spkbehavior;
}

/*
 * Decreases the speaker behavior setting
 * Returns new speaker behavior
 */
enum pre_spkbehavior pre_decreasespkbehavior() {
	switch(ram_spkbehavior) {
	case SPK_AUTO:
		ram_spkbehavior = SPK_ON;
		break;
	case SPK_ON:
		ram_spkbehavior = SPK_OFF;
		break;
	case SPK_OFF:
		ram_spkbehavior = SPK_AUTO;
		break;
	}
	return ram_spkbehavior;
}

/*
 * Gets the speaker behavior
 */
enum pre_spkbehavior pre_getspkbehavior() {
	return ram_spkbehavior;
}

/*
 * gets the current input number
 * zero to (PRE_NINPUTS - 1)
 */
uint8_t pre_getcurrentinput() {
	return ram_inselect;
}

/*
 * jumps to the next input
 * returns new input number
 */
uint8_t pre_nextinput() {
	ram_inselect++;
	if(ram_inselect >= PRE_NINPUTS) ram_inselect = 0;
	//eeprom_update_byte(&ee_inselect, currentinput);
	// TODO: implement hardware input select
	return ram_inselect;
}

/*
 * jumps to previous (lower) input
 * returns new input number
 */
uint8_t pre_previnput() {
	if(ram_inselect > 0) {
		ram_inselect--;
	} else {
		ram_inselect = PRE_NINPUTS - 1;
	}
	// TODO: implement hardware input select
	return ram_inselect;
}

/*
 * Increments volume.
 * returns new volume.
 */
uint8_t pre_increasevol() {
	ram_volume++;
	if(ram_volume > PRE_MAXVOL) ram_volume = PRE_MAXVOL;
	// TODO: implement hardware potentiometer control
	return ram_volume;
}

/*
 * Decrements volume.  Volume min is 0.
 * returns new volume.
 */
uint8_t pre_decreasevol() {
	if(ram_volume > 0) ram_volume--;
	// TODO: implement hardware potentiometer control
	return ram_volume;
}
