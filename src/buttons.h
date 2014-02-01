/*
 * buttons.h - Handle button presses both local and remote
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

#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <stdint.h>
#include "pins.h"

// Note: these numeric definitions are essential to simplify things in buttons.c
enum but_type {
    BUT_ENTER   = (1<<PINC_ENTER),  // Enter button
    BUT_BACK    = (1<<PINC_BACK),   // Back button
    BUT_VOLUP   = (1<<PINC_VOLUP),  // Volume up/+ button
    BUT_VOLDN   = (1<<PINC_VOLDN),  // Volume down/- button
    BUT_LEFT    = (1<<PINC_LEFT),   // Select left button
    BUT_RIGHT   = (1<<PINC_RIGHT),  // Select right button
    BUT_NONE    = 0                  // No button press
};

// sets up inputs (TBI: Timer)
void butinit();

// tie a function to the button interrupts
void but_setint(void (*f)());

// Waits for and debounces a press
enum but_type but_getaction();

// returns nonzero if any buttons are pressed
uint8_t but_ispressed();


#endif
