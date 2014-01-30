/*
 * inputnames.h
 * Author Ali Kocaturk 1/4/14
 */
#ifndef INPUTNAMES_H_
#define INPUTNAMES_H_
 
#include <stdint.h>

void name_get(char * buf, uint8_t name_num);

void name_put(const char * newname, uint8_t name_num);

void name_getprefix(char * buf, uint8_t name_num);

#endif
