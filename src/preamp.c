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
 *
 * This code combines high-level functions (saving and editing) with low-level
 * I/O. For that reason, it may be fairly complex.
 *
 *
 * Hardware considerations:
 *
 * This is intended to drive three MCP42xxx devices daisy-chained together.
 * The devices should be chained together as shown below.
 *    _____              _____        ______        ______
 *   | AVR |MOSI      SI| VOL |SO  SI| BASS |SO  SI| TREB |SO
 *   |_____|------------|_____|------|______|------|______|X
 *
 * Of course, all three digital potentiometers should be connected to
 * the same SCK pin of the AVR.
 *
 * TODO: Headphone detection and speaker control are not yet implemented.
 * TODO: Tone behavior
 */

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "pins.h"
#include "preamp.h"
#include "spi.h"

#define PRE_NINPUTS 8
#define PRE_MINTONE (-12)
#define PRE_MAXTONE 12
#define PRE_MAXVOL 38

#define POT_NOP 0
#define POT_WRITE (1<<4)
#define POT_BOTH 3

// logarithmic volume curve
// as determined in "/misc/volume pot curve.xlsx"
static const uint8_t PROGMEM pre_volcurve[39] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 
	18, 20, 22, 25, 28, 32, 36, 40, 45, 50, 57, 64, 71,
	80, 90, 101, 113, 127, 143, 160, 180, 202, 227, 255
};

// linear tone control curve
// to avoid complicated math
static const uint8_t PROGMEM pre_tonecurve[25] = {
	0, 11, 21, 32, 43, 53, 64, 74, 85, 96, 106, 117, 128, 
	138, 149, 159, 170, 181, 191, 202, 213, 223, 234, 244, 255
};

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
static void pre_updatepots();

void preinit() {
	PORTB |= 1<<POT_CS;	// Pot CS is high
	DDRB |= 1<<POT_CS;	// pot CS is output
	
	pre_updatepots();	// set pots to their default values
	
	pre_load();
	
	_delay_ms(3000);	// wait for caps to charge
	
	pre_updatepots();	// set pots to loaded volume settings
}

/*
 * Loads all preamp settings from EEPROM
 */
static void pre_load() {
	ram_volume = eeprom_read_byte(&ee_volume);
	ram_inselect = eeprom_read_byte(&ee_inselect);
	eeprom_read_block(&ram_bass, &ee_bass, sizeof(ee_bass));
	eeprom_read_block(&ram_treb, &ee_treb, sizeof(ee_treb));
	eeprom_read_block(&ram_tonebehavior, &ee_tonebehavior, sizeof(ee_tonebehavior));
	eeprom_read_block(&ram_spkbehavior, &ee_spkbehavior, sizeof(ee_spkbehavior));
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
	pre_updatepots();
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
	pre_updatepots();
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
	pre_updatepots();
	return ram_bass;
}

/*
 * decreases the bass setting, if possible
 * returns the new bass setting
 */
int8_t pre_decreasebass() {
	ram_bass--;
	if(ram_bass < PRE_MINTONE) ram_bass = PRE_MINTONE;
	pre_updatepots();
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
	pre_updatepots();
	return ram_treb;
}

/*
 * decreases the treble setting, if possible
 * returns the new treble setting
 */
int8_t pre_decreasetreb() {
	ram_treb--;
	if(ram_treb < PRE_MINTONE) ram_treb = PRE_MINTONE;
	pre_updatepots();
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
	pre_updatepots();
	return ram_volume;
}

/*
 * Decrements volume.  Volume min is 0.
 * returns new volume.
 */
uint8_t pre_decreasevol() {
	if(ram_volume > 0) ram_volume--;
	pre_updatepots();
	return ram_volume;
}

/*
 * Updates the pots according to the ram_vol, ram_bass, and ram_treb settings
 * TODO: Implement tone behavior
 */
static void pre_updatepots() {
	uint8_t volpot_val, trebpot_val, basspot_val;// stores calculated pot codes
	
	// convert ram_vol, ram_bass, ram_treb to pot codes
	volpot_val = pgm_read_byte(&(pre_volcurve[ram_volume]));
	trebpot_val = pgm_read_byte(&(pre_tonecurve[ram_treb]));
	basspot_val = pgm_read_byte(&(pre_tonecurve[ram_bass]));
	
	// push values out to pots
	spiinit(SPI_MSBFIRST, SPI_MODE0, SPI_CKDIV4);// set pot spi mode
	PORTB &= ~(1<<POT_CS);	// enable pot CS
	spi_transfer(POT_WRITE|POT_BOTH);
	spi_transfer(trebpot_val);	// send to pot 3 (treb)
	spi_transfer(POT_WRITE|POT_BOTH);
	spi_transfer(basspot_val);	// send to pot 2 (bass)
	spi_transfer(POT_WRITE|POT_BOTH);
	spi_transfer(volpot_val);	// send to pot 1 (vol)
	PORTB |= 1<<POT_CS;		// disable pot CS
}