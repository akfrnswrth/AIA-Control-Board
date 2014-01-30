/*
 * preamp.h
 * handles preamp & amp control
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
