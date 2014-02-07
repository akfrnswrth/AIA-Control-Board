/*
 * spi.c - SPI interface code for AIA Control Board
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
 * SPI master library
 */ 

#include <avr/io.h>
#include "spi.h"
#include "pins.h"

void spiinit(uint8_t bit_order, uint8_t data_mode, uint8_t speed) {
	DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK);	// MOSI and SCK are output since this chip is master
	SPCR = (1<<SPE)|bit_order|(1<<MSTR)|data_mode|speed;
}

char spi_transfer(char c) {
	char r;
	SPCR |= 1<<MSTR;				// ensure we're in master mode
	DDRB |= 1<<SPI_SCK;				// ensure SCK is hard output
	SPDR = c;						// initiate transfer
	while((SPSR & (1<<SPIF)) == 0);// wait for transfer to complete
	r = SPDR;						// get received byte
	return r;
}
