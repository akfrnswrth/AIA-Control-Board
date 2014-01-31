/*
 * pins.h - Pin definitions for AIA Control Board with ATmega168/328
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


#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

// on PORTB
#define SPI_SCK 5
#define SPI_MOSI 3
#define SPI_MISO 4
#define VFD_CS 1
#define EE_LED 0

// on PORTC
// Prefix should be changed to PINC
#define PINC_ENTER 0
#define PINC_BACK 1
#define PINC_VOLUP 2
#define PINC_VOLDN 3
#define PINC_LEFT 4
#define PINC_RIGHT 5
#define PINC_BUTMASK ((1<<PINC_ENTER)|(1<<PINC_BACK)| \
			  	 (1<<PINC_VOLUP)|(1<<PINC_VOLDN)| \
			  	 (1<<PINC_LEFT)|(1<<PINC_RIGHT))

// PCINT for PORTC
// Prefix should be changed to PCI1
#define PCI1_ENTER PCINT8
#define PCI1_BACK PCINT9
#define PCI1_VOLUP PCINT10
#define PCI1_VOLDN PCINT11
#define PCI1_LEFT PCINT12
#define PCI1_RIGHT PCINT13
#define PCI1_MASK ((1<<PCI1_ENTER)|(1<<PCI1_BACK)| \
				(1<<PCI1_VOLUP)|(1<<PCI1_VOLDN)| \
				(1<<PCI1_LEFT)|(1<<PCI1_RIGHT))

#endif /* PINS_H_ */
