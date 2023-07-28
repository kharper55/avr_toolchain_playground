/***********************************************************************
* Hardware Abstraction for AVR Microcontrollers GPIO Driver            *
* @author Ahmed Elzoughby                                              *
* @date July 26, 2017                                                  *
* Purpose: Provide an easy interface to deal with digital I/O devices  *
*																	   *
* Adapated by Kevin Harper for the ATMega328P 07/13/2023			   *
***********************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "gpio_types.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

typedef enum gpio_error {
	GPIO_OK,
	GPIO_INVALID_PORT,
	GPIO_INVALID_PIN,
	GPIO_INVALID_DIR,
	GPIO_INVALID_PIN_STATE,
	GPIO_INVALID_MCUCR
} gpio_error_t;

typedef enum gpio_dir {
	DIR_OUTPUT = 1,
	DIR_INPUT = 0,
	DIR_INPUT_PULLUP = 2
} gpio_dir_t;

typedef enum gpio_port {
	GPIO_B,
	GPIO_C,
	GPIO_D
} gpio_port_t;

#define OFFSET_PIN  0x00
#define OFFSET_DIR  0x01
#define OFFSET_PORT 0x02

//gpio_error_t gpio_pin_init(uint8_t base_addr, uint8_t pin_num, gpio_dir_t dir);
//gpio_error_t gpio_port_init(uint8_t base_addr, gpio_dir_t dir);

gpio_error_t gpio_port_init(gpio_port_t port, gpio_dir_t dir, uint8_t mask);
gpio_error_t gpio_pin_init(gpio_port_t port, gpio_dir_t dir, uint8_t pin);

gpio_error_t gpio_pin_read(gpio_port_t port, uint8_t pin, bit_t* result);
gpio_error_t gpio_port_read(gpio_port_t port, uint8_t* result, uint8_t mask);

gpio_error_t gpio_pin_write(gpio_port_t port, uint8_t pin, bit_t state);
gpio_error_t gpio_port_write(gpio_port_t port, uint8_t val, uint8_t mask);

#endif //GPIO_H_
