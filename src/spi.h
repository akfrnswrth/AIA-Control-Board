/*
 * spi.h - SPI interface code for AIA Control Board
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


#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <avr/io.h>

#define SPI_MSBFIRST (0<<DORD)
#define SPI_LSBFIRST (1<<DORD)
#define SPI_MODE0 ((0<<CPOL)|(0<<CPHA))
#define SPI_MODE1 ((0<<CPOL)|(1<<CPHA)) 
#define SPI_MODE2 ((1<<CPOL)|(0<<CPHA))
#define SPI_MODE3 ((1<<CPOL)|(1<<CPHA))
#define SPI_CKDIV4 ((0<<SPR1)|(0<<SPR0))
#define SPI_CKDIV16 ((0<<SPR1)|(1<<SPR0))
#define SPI_CKDIV64 ((1<<SPR1)|(0<<SPR0))
#define SPI_CKDIV128 ((1<<SPR1)|(1<<SPR0))

/* Shifts out data c, returning any data shifted in.
   Leaves SPI_SCK input, MOSI output, MISO untouched */
char spi_transfer(char c);

/* Initializes SPI bus.  Leaves SPI_SCK input, MOSI output, MISO untouched. 
   bit_order can be SPI_MSBFIRST or SPI_LSBFIRST.  data_mode can be SPI_MODE0,
   SPI_MODE1, SPI_MODE2, or SPI_MODE3.  */
void spiinit(uint8_t bit_order, uint8_t data_mode, uint8_t speed);

#endif /* SPI_H_ */
