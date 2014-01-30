/*
 * spi.c
 *
 * Created: 1/3/2014 11:29:42 PM
 *  Author: Ali
 * SPI interface code for AIA Control Board
 */ 

#include <avr/io.h>
#include "spi.h"
#include "pins.h"

void spiinit(uint8_t bit_order, uint8_t data_mode, uint8_t speed) {
	DDRB |= (1<<SPI_MOSI);
	DDRB &= ~(1<<SPI_SCK);
	PORTB |= 1<<SPI_SCK;
	SPCR = (1<<SPE)|bit_order|(1<<MSTR)|data_mode|speed;
}

char spi_transfer(char c) {
	char r;
	SPCR |= 1<<MSTR;				// ensure we're in master mode
	DDRB |= 1<<SPI_SCK;				// SCK is now output
	SPDR = c;						// initiate transfer
	while((SPSR & (1<<SPIF)) == 0);	// wait for transfer to complete
	DDRB &=~(1<<SPI_SCK);			// SCK is now input
	r = SPDR;						// get received byte
	return r;
}
