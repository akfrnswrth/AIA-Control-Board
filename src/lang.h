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

extern const char LANG_VOLUME[] PROGMEM;

extern const char LANG_TC[] PROGMEM;
extern const char LANG_TC_SPKONLY[] PROGMEM;
extern const char LANG_TC_ALWAYS[] PROGMEM;
extern const char LANG_TC_NEVER[] PROGMEM;
extern const char LANG_TC_BASS[] PROGMEM;
extern const char LANG_TC_TREBLE[] PROGMEM;

extern const char LANG_SPK_AUTO[] PROGMEM;
extern const char LANG_SPK_ON[] PROGMEM;
extern const char LANG_SPK_OFF[] PROGMEM;

extern const char LANG_INNAMES[] PROGMEM;
extern const char LANG_IN0[] PROGMEM;
extern const char LANG_IN1[] PROGMEM;
extern const char LANG_IN2[] PROGMEM;
extern const char LANG_IN3[] PROGMEM;
extern const char LANG_IN4[] PROGMEM;
extern const char LANG_IN5[] PROGMEM;
extern const char LANG_IN6[] PROGMEM;
extern const char LANG_IN7[] PROGMEM;

extern const char LANG_BRIGHTNESS[] PROGMEM;
extern const char LANG_ACTIVEBRIGHTNESS[] PROGMEM;
extern const char LANG_IDLEBRIGHTNESS[] PROGMEM;

extern const char ERROR_CANTHAPPEN[] PROGMEM;

#endif /* LANG_H_ */
