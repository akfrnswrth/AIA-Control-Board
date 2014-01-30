/*
 * lang.h - Language string definitions for AIA control board
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
 */ 
 
#include <avr/pgmspace.h>
#include "lang.h"

const char PROGMEM LANG_SPLASH[] 		= "AIA Rev. B";

const char PROGMEM LANG_VOLUME[] 		= "Volume: %hhu";	// it's a format string

const char PROGMEM LANG_TC[] 			= "Tone>";
const char PROGMEM LANG_TC_SPKONLY[] 	= 	"Active: Spk only";
const char PROGMEM LANG_TC_ALWAYS[] 	=   "Active: Always";
const char PROGMEM LANG_TC_NEVER[] 		=   "Active: Never";
const char PROGMEM LANG_TC_BASS[] 		=   "Bass: %+hhd";
const char PROGMEM LANG_TC_TREBLE[] 	=   "Treb: %+hhd";

const char PROGMEM LANG_SPK_AUTO[] 		= "Speakers: Auto";
const char PROGMEM LANG_SPK_ON[] 		= "Speakers: Always";
const char PROGMEM LANG_SPK_OFF[] 		= "Speakers: Off";

const char PROGMEM LANG_INNAMES[] 		= "Input Names>";
const char PROGMEM LANG_IN0[] 			=   "AU: ";	// NOT format strings
const char PROGMEM LANG_IN1[] 			=   "L1: ";
const char PROGMEM LANG_IN2[] 			=   "L2: ";
const char PROGMEM LANG_IN3[] 			=   "L3: ";
const char PROGMEM LANG_IN4[] 			=   "L4: ";
const char PROGMEM LANG_IN5[] 			=   "L5: ";
const char PROGMEM LANG_IN6[] 			=   "L6: ";
const char PROGMEM LANG_IN7[]  			=   "L7: ";

const char PROGMEM LANG_BRIGHTNESS[] 	= "Brightness>";
const char PROGMEM LANG_ACTIVEBRIGHTNESS[] = "Active Bright: %hhu";
const char PROGMEM LANG_IDLEBRIGHTNESS[] =	"Idle Bright: %hhd";

const char PROGMEM ERROR_CANTHAPPEN[]	= "E: Can't happen";
