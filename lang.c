/*
 * lang.c
 * Author Ali Kocaturk 1/6/14
 * String definitions for AIA Control Board
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
