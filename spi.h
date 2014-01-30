/*
 * spi.h
 *
 * Created: 1/3/2014 8:31:22 PM
 *  Author: Ali
 * SPI interface header
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
