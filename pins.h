/*
 * pins.h
 *
 * Created: 1/3/2014 7:31:42 PM
 *  Author: Ali
 * 
 * Pin definitions for AIA Control Board with ATmega168/328
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
#define BUT_ENTER 0
#define BUT_BACK 1
#define BUT_VOLUP 2
#define BUT_VOLDN 3
#define BUT_LEFT 4
#define BUT_RIGHT 5
#define BUT_MASK ((1<<BUT_ENTER)|(1<<BUT_BACK)| \
			  	 (1<<BUT_VOLUP)|(1<<BUT_VOLDN)| \
			  	 (1<<BUT_LEFT)|(1<<BUT_RIGHT))

// PCINT for PORTC
// Prefix should be changed to PCI1
#define PC_ENTER PCINT8
#define PC_BACK PCINT9
#define PC_VOLUP PCINT10
#define PC_VOLDN PCINT11
#define PC_LEFT PCINT12
#define PC_RIGHT PCINT13
#define PC_MASK ((1<<BUT_ENTER)|(1<<BUT_BACK)| \
				(1<<BUT_VOLUP)|(1<<BUT_VOLDN)| \
				(1<<BUT_LEFT)|(1<<BUT_RIGHT))

//// on PORTD
//#define REM_RX 0	// deprecated; part of code emeritus

#endif /* PINS_H_ */
