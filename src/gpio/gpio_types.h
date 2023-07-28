/***********************************************************************
* Custom types for AVR Microcontrollers GPIO Driver                    *
* @author Ahmed Elzoughby                                              *
* @date July 26, 2017                                                  *
* Purpose: Provide an easy interface to deal with digital I/O devices  *
*										    						   *
* Adapated by Kevin Harper for the ATMega328P 07/13/2023			   *
***********************************************************************/

#ifndef GPIO_TYPES_H_
#define GPIO_TYPES_H_

#include <stdint.h>

typedef enum bit {
	LOW = 0,
	HIGH = 1,
	ZERO = 0,
	ONE = 1,
	OFF = 0,
	ON = 1
} bit_t;

#endif //TYPES_H_
