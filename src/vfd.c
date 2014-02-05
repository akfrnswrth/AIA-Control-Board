/*
 * vfd.c - VFD control code for AIA Control Board
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

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "pins.h"
#include "vfd.h"
#include "spi.h"

// EEPROM brightness data
static uint8_t EEMEM ee_activebrightness = 8;
static uint8_t EEMEM ee_idlebrightness = 1;

// volatile RAM copies
static volatile uint8_t ram_activebrightness = 8;
static volatile uint8_t ram_idlebrightness = 1;

static void vfd_load();

void vfdinit() {
	DDRB |= (1<<VFD_CS);	// chip select (CS) is output
	PORTB |= (1<<VFD_CS);	// CS is high (disabled) until putd()
	
	vfd_putd(VFD_POWEROFF);	// blank the display
	vfd_putd(VFD_SETLENGTH | 0x07);	// 16 digits
	vfd_putd(VFD_SETDIMMER | 0x07); // full brightness to ensure lit display
	vfd_putd(0xF7);			// fast display frequency
	vfd_putd(VFD_POWERON);	// turn on display
	vfd_putd(VFD_SETINCREM | 0x01);	// enable auto-increment (should already be enabled, usually) 
	vfd_putd(VFD_SETCURSOR);	// reset cursor
	
	vfd_clear();
	
	vfd_load();					// load brightness values from EEPROM
	
	//return &vfd_vfdstream;
}

uint8_t vfd_getactivebrightness() {
	return ram_activebrightness;
}

uint8_t vfd_getidlebrightness() {
	return ram_idlebrightness;
}

void vfd_save() {
	PORTB |= 1<<EE_LED;		// turn on EEPROM access LED
	eeprom_update_byte(&ee_activebrightness, ram_activebrightness);
	eeprom_update_byte(&ee_idlebrightness, ram_idlebrightness);
	PORTB &= ~(1<<EE_LED);	// turn off EEPROM access LED
}

static void vfd_load() {
	ram_activebrightness = eeprom_read_byte(&ee_activebrightness);
	ram_idlebrightness = eeprom_read_byte(&ee_idlebrightness);
}

void vfd_putd(char d) {
	spiinit(SPI_MSBFIRST, SPI_MODE3, SPI_CKDIV4);// set VFD spi mode
	PORTB &= ~(1<<VFD_CS);	// select VFD chipselect
	spi_transfer(d);		// shift out data
	while((PINB & (1<<SPI_SCK)) == 0);	// wait for VFD to release clock line
}

int vfd_putchar(char c) {
	if(c < 0x20 || c > 0x7f) c = 0x7f;	// make invalid chars obvious
	vfd_putd(c);
	PORTB |= (1<<VFD_CS);	// clear VFD chipselect
	return 0;
}

void vfd_setcursor(uint8_t cursor_pos) {
	cursor_pos &= 0x0f;		// protect from invalid cursor_pos
	vfd_putd(VFD_SETCURSOR | cursor_pos);
	PORTB |= (1<<VFD_CS);	// clear VFD chipselect
}

void vfd_setbrightness(uint8_t brightness) {
	if(brightness == 0) {
		vfd_putd(VFD_POWEROFF);	// blank the display at brightness 0
	} else {
		vfd_putd(VFD_POWERON);	// make sure display is on at nonzero brightness
		brightness = (brightness - 1) & 0x07;		// convert to dimmer value
		vfd_putd(VFD_SETDIMMER | brightness);
	}
	PORTB |= (1<<VFD_CS);	// clear VFD chipselect
}

void vfd_activebrightness() {
	vfd_setbrightness(ram_activebrightness);
}

uint8_t vfd_increaseactivebrightness() {
	if(ram_activebrightness < 8) ram_activebrightness++;
	vfd_setbrightness(ram_activebrightness);
	return ram_activebrightness;
}

uint8_t vfd_decreaseactivebrightness() {
	if(ram_activebrightness > 1) ram_activebrightness--;		// minimum active brightness is 1
	vfd_setbrightness(ram_activebrightness);
	return ram_activebrightness;
}

void vfd_idlebrightness() {
	vfd_setbrightness(ram_idlebrightness);
}

uint8_t vfd_increaseidlebrightness() {
	if(ram_idlebrightness < 8) ram_idlebrightness++;
	return ram_idlebrightness;
}

uint8_t vfd_decreaseidlebrightness() {
	if(ram_idlebrightness > 0) ram_idlebrightness--;
	return ram_idlebrightness;
}


void vfd_clear() {
	vfd_setcursor(0);
	for(int i = 0; i < 16; i++) {
		vfd_putd(' ');
	}
	vfd_setcursor(0);	// vfd_setcursor releases chipselect
}

// updates display quickly
void update_display(const char * msg) {
	uint8_t i = 0;
	vfd_setcursor(0);
	while(i < 16 && msg[i] != 0) {
		vfd_putchar(msg[i]);
		i++;
	}
	while(i < 16) {
		vfd_putchar(' ');
		i++;
	}
}

// updates display quickly from program space
void update_display_P(PGM_P msg_P) {
	char msg[17];
	strncpy_P(msg, msg_P, 17);
	update_display(msg);
}

// displays msg centered
void center_display(const char * msg) {
	uint8_t i;
	uint8_t l = strlen(msg);
	uint8_t o = (16 - l) / 2;
	vfd_setcursor(0);
	for(i = 0; i < 16; i++) {
		if (i >= o && i - o < l) {
			vfd_putchar(msg[i - o]);
		} else {
			vfd_putchar(' ');
		}
	}
}

// displays centered from program space
void center_display_P(PGM_P msg_P) {
	char msg[17];
	strncpy_P(msg, msg_P, 17);
	center_display(msg);
}
