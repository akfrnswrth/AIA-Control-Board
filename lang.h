/*
 * lang.h - Language string declarations for AIA control board
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


#ifndef LANG_H_
#define LANG_H_

#include <avr/pgmspace.h>

extern const char LANG_SPLASH[] PROGMEM;

// TODO: use PSTR instead of these long types
extern const char LANG_VOLUME[] PROGMEM;// 		= "Volume: %d";	// it's a format string

extern const char LANG_TC[] PROGMEM;// 			= "Tone>";
extern const char LANG_TC_SPKONLY[] PROGMEM;// 	= 	"Active: Spk only";
extern const char LANG_TC_ALWAYS[] PROGMEM;// 	=   "Active: Always";
extern const char LANG_TC_NEVER[] PROGMEM;// 		=   "Active: Never";
extern const char LANG_TC_BASS[] PROGMEM;// 		=   "Bass: %+hhd";
extern const char LANG_TC_TREBLE[] PROGMEM;// 	=   "Treb: %+hhd";

//extern const char LANG_SPK[] PROGMEM;// 			= "Speakers";
extern const char LANG_SPK_AUTO[] PROGMEM;// 		= "Speakers: Auto";
extern const char LANG_SPK_ON[] PROGMEM;// 		= "Speakers: Always";
extern const char LANG_SPK_OFF[] PROGMEM;// 		= "Speakers: Off";

extern const char LANG_INNAMES[] PROGMEM;// 		= "Input Names>";
extern const char LANG_IN0[] PROGMEM;// 			=   "PH: ";	// NOT format strings
extern const char LANG_IN1[] PROGMEM;// 			=   "L1: ";
extern const char LANG_IN2[] PROGMEM;// 			=   "L2: ";
extern const char LANG_IN3[] PROGMEM;// 			=   "L3: ";
extern const char LANG_IN4[] PROGMEM;// 			=   "L4: ";
extern const char LANG_IN5[] PROGMEM;// 			=   "L5: ";
extern const char LANG_IN6[] PROGMEM;// 			=   "L6: ";
extern const char LANG_IN7[] PROGMEM;//  			=   "AU: ";

extern const char LANG_BRIGHTNESS[] PROGMEM;
extern const char LANG_ACTIVEBRIGHTNESS[] PROGMEM;
extern const char LANG_IDLEBRIGHTNESS[] PROGMEM;

extern const char ERROR_CANTHAPPEN[] PROGMEM;

//extern const char LANG_REM_UNKNOWN[] PROGMEM;// = "remcode %d";	// remote control has been retired

#endif /* LANG_H_ */
