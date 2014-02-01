/*
 * preamp.h - Preamp functions
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
 * (vol, treb, bass, speaker select)
 */

#ifndef PREAMP_H_
#define PREAMP_H_

enum pre_spkbehavior {
	SPK_AUTO,
	SPK_ON,
	SPK_OFF
};

enum pre_tonebehavior {
	TONE_SPKONLY,
	TONE_ALWAYS,
	TONE_NEVER
};

/*
 * Performs any necessary initialization and calls pre_load();
 */
void preinit();

/*
 * Saves configuration (volume, tone, input, behavior, etc) to EEPROM
 */
void pre_save();

/*
 * Increments volume. volume max is 64
 * returns new volume.
 */
uint8_t pre_increasevol();

/*
 * Decrements volume.  Volume min is 0.
 * returns new volume.
 */
uint8_t pre_decreasevol();

/*
 * jumps to the next input
 * returns new input number
 */
uint8_t pre_nextinput();

/*
 * jumps to previous (lower) input
 * returns new input number
 */
uint8_t pre_previnput();

/*
 * gets the current input number
 * zero to (PRE_NINPUTS - 1)
 */
uint8_t pre_getcurrentinput();

/*
 * gets the bass setting
 */
int8_t pre_getbass();

/*
 * increases the bass setting, if possible
 * returns the new bass setting
 */
int8_t pre_increasebass();

/*
 * decreases the bass setting, if possible
 * returns the new bass setting
 */
int8_t pre_decreasebass();

/*
 * gets the treble setting
 */
int8_t pre_gettreb();

/*
 * increases the treble setting, if possible
 * returns the new bass setting
 */
int8_t pre_increasetreb();

/*
 * decreases the treble setting, if possible
 * returns the new treble setting
 */
int8_t pre_decreasetreb();

/*
 * Gets the tone behavior
 */
enum pre_tonebehavior pre_gettonebehavior();

/*
 * Increases the tone behavior setting
 * Returns new tone behavior
 */
enum pre_tonebehavior pre_increasetonebehavior();

/*
 * Decreases the tone behavior setting
 * Returns new tone behavior
 */
enum pre_tonebehavior pre_decreasetonebehavior();

/*
 * Increases the speaker behavior setting
 * Returns new speaker behavior
 */
enum pre_spkbehavior pre_increasespkbehavior();

/*
 * Decreases the speaker behavior setting
 * Returns new speaker behavior
 */
enum pre_spkbehavior pre_decreasespkbehavior();

/*
 * Gets the speaker behavior
 */
enum pre_spkbehavior pre_getspkbehavior();

#endif
