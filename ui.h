/*
 * ui.h
 * The UI (menu) for AIA Control Board
 */
 
#ifndef UI_H_
#define UI_H_

#include <avr/io.h>
#include <stdint.h>

// these are the only non-static functions... everything else is internal to ui.c
void uiinit();
void uiloop();

#endif
