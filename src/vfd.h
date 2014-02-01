/*
 * vfd.h - VFD control code for AIA Control Board
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
 * specifically for CU165ECPB-T2J vacuum fluorescent display
 * should work for most M66004-based serial VFDs
 */ 

#ifndef VFD_H_
#define VFD_H_

#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "pins.h"

#define VFD_SETLENGTH 0x00
#define VFD_SETDIMMER 0x08
#define VFD_SETCURSOR 0xe0
#define VFD_SETINCREM 0xf4

#define VFD_SETCOMMAON 0x10
#define VFD_SETCOMMAOFF 0x80

#define VFD_POWERON 0xf1
#define VFD_POWEROFF 0xf0

#define VFD_SETUSERCHAR 0xfc

// sets up VFD to sane settings
void vfdinit();

// putchar function to send characters individually
int vfd_putchar(char c/*, FILE * stream*/);

/* direct put function
   WARNING: This is for putting data to the VFD, and thus
   doesn't release chipselect.  Be sure to release (make high) VFD_CS
   before using the SPI bus for other purposes. The only
   reason this should be used outside of vfd.c is to load
   custom characters. */
void vfd_putd(char d);

// moves cursor to specified location
void vfd_setcursor(uint8_t cursor_pos);

// sets the brightness of the VFD.  Valid values are 0 to 8.
void vfd_setbrightness(uint8_t brightness);

// sets the display to active brightness
void vfd_activebrightness();

// sets the display to idle brightness
void vfd_idlebrightness();

// increases the active brightness of the VFD, if possible.  Returns new active brightness
uint8_t vfd_increaseactivebrightness();

// decreases the active brightness of the VFD, if possible.  Returns new active brightness
uint8_t vfd_decreaseactivebrightness();

// gets the active brightness of the VFD
uint8_t vfd_getactivebrightness();

// increases the idle brightness of the VFD, if possible.  Returns new idle brightness
uint8_t vfd_increaseidlebrightness();

// decreases the idle brightness of the VFD, if possible.  Returns new idle brightness
uint8_t vfd_decreaseidlebrightness();

// gets the idle brightness of the VFD
uint8_t vfd_getidlebrightness();

// saves the brightnesses of the VFD
void vfd_save();

/* clears display by overwriting all locations with space, then 
   puts cursor at zero */
void vfd_clear();

// overwrites the display quickly
void update_display(const char *);

// prints a message centered on the display
void center_display(const char *);

// same as update_display() but takes a string from program space
void update_display_P(PGM_P msg_P);

// same as center_display() but takes a string from program space
void center_display_P(PGM_P msg_P);

#endif // VFD_H_
