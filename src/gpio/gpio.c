/***********************************************************************
* Hardware Abstraction for AVR Microcontrollers GPIO Driver            *
* @author Ahmed Elzoughby                                              *
* @date July 26, 2017                                                  *
* Purpose: Provide an easy interface to deal with digital I/O devices  *
* 																	   *
* Adapated by Kevin Harper for the ATMega328P 07/13/2023			   *
***********************************************************************/

#include "gpio.h"

// try to break some of this out into static functions

// add optional interrupt functionality

// pins a6/a7 dont have digital drivers behind them so cant be used idnetically to others

// lean out the functions and add in an option to reinit an already initialized port 
// this would be necessary for bitbanging a protocol and/or using a trio of gpio 
// pins for a semiconductor tester/indentification device
// when switching between input and output, an intermediate state must occur

gpio_error_t gpio_port_init(gpio_port_t port, gpio_dir_t dir, uint8_t mask) {
	// Accept PORT avr-gnu macros as inputs for selecting which gpio port to use
    // Check if requested direction is valid
	// Set DDR bits to 1
	switch(port) {
		case(GPIO_B):
			if (dir == DIR_OUTPUT) DDRB |= (uint8_t)(0xFF & mask);
			else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
				DDRB &= (uint8_t)(0x00 | ~mask);
				if (dir == DIR_INPUT_PULLUP) PORTB |= (uint8_t)(0xFF & mask);
			}
			else return GPIO_INVALID_DIR;
			break;
		case(GPIO_C):
			if (dir == DIR_OUTPUT) DDRC |= (uint8_t)((0xFF & mask) >> 1);
			else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
				DDRC &= (uint8_t)((0x00 | ~mask) >> 1);
				if (dir == DIR_INPUT_PULLUP) PORTC |= (uint8_t)((0xFF & mask) >> 1);
			}
			else return GPIO_INVALID_DIR;
			break;
		case(GPIO_D):
			if (dir == DIR_OUTPUT) DDRD |= (uint8_t)(0xFF & mask);
			else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
				DDRD &= (uint8_t)(0x00 | ~mask);
				if (dir == DIR_INPUT_PULLUP) PORTD |= (uint8_t)(0xFF & mask);
			}
			else return GPIO_INVALID_DIR;
			break;
		default: 
			return GPIO_INVALID_PORT;
	}
	
	return GPIO_OK;
}


gpio_error_t gpio_pin_init(gpio_port_t port, gpio_dir_t dir, uint8_t pin) {
	// Accept PORT avr-gnu macros as inputs for selecting which gpio port to use
    // Check if requested direction is valid
	// Set DDR bits to 1
	switch(port) {
		case(GPIO_B):
			if (pin < 8) {
				if (dir == DIR_OUTPUT) DDRB |= (uint8_t)(1 << pin);
				else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
					DDRB &= ~(uint8_t)(1 << pin);
					if ((dir == DIR_INPUT_PULLUP) && ((MCUCR & (1 << PUD)) == 0)) {
						PORTB |= (uint8_t)(1 << pin);
					}
					else return GPIO_INVALID_MCUCR;
				}
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;
			break;
		case(GPIO_C):
			if (pin < 7) {
				if (dir == DIR_OUTPUT) DDRC |= (uint8_t)(1 << pin);
				else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
					DDRC &= ~(uint8_t)(1 << pin);
					if ((dir == DIR_INPUT_PULLUP) && ((MCUCR & (1 << PUD)) == 0)) {
						PORTC |= (uint8_t)(1 << pin);
					}
					else return GPIO_INVALID_MCUCR;
				}
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;
			break;
		case(GPIO_D):
			if (pin < 8) {
				if (dir == DIR_OUTPUT) DDRD |= (uint8_t)(1 << pin);
				else if (dir == DIR_INPUT || dir == DIR_INPUT_PULLUP) {
					DDRD &= ~(uint8_t)(1 << pin);
					if ((dir == DIR_INPUT_PULLUP) && ((MCUCR & (1 << PUD)) == 0)) {
						PORTD |= (uint8_t)(1 << pin);
					}
					else return GPIO_INVALID_MCUCR;
				}
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;	
			break;	
		default:
			return GPIO_INVALID_PORT;
	}
	
	return GPIO_OK;
}

// this mask is only to allow writes when the entire port is not initialized,
// but since assignment operation used, 0's wil be written possibly in port bits
// that are uninitialized
gpio_error_t gpio_port_write(gpio_port_t port, uint8_t val, uint8_t mask) {
	switch(port) {
		case(GPIO_B):
			if (DDRB == (0xFF & mask) || DDRB == 0xFF) PORTB = val;
			else return GPIO_INVALID_DIR;
			break;
		case(GPIO_C):
			if (DDRC == (0x7F & mask) || DDRC == 0x7F) PORTC = val;
			else return GPIO_INVALID_DIR;
			break;
		case(GPIO_D):
			if (DDRD == (0xFF & mask) || DDRD == 0xFF) PORTD = val;
			else return GPIO_INVALID_DIR;
			break;
		default:
			return GPIO_INVALID_PORT;

	}

	return GPIO_OK;
}

gpio_error_t gpio_pin_write(gpio_port_t port, uint8_t pin, bit_t state) {
	switch(port) {
		case(GPIO_B):
			if(pin < 8) {
				if (((DDRB & (1 << pin)) >> pin) == 1) {
					if (state == HIGH) PORTB |= (1 << pin);
					else if (state == LOW) PORTB &= ~(1 << pin);
					else return GPIO_INVALID_PIN_STATE;
				} 
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;
			break;
		case(GPIO_C):
			if (pin < 7) {
				if (((DDRC & (1 << pin)) >> pin) == 1) {
					if (state == HIGH) PORTC |= (1 << pin);
					else if (state == LOW) PORTC &= ~(1 << pin);
					else return GPIO_INVALID_PIN_STATE;
				} 
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;
			break;
		case(GPIO_D):
			if (pin < 8) {
				if (((DDRD & (1 << pin)) >> pin) == 1) {
					if (state == HIGH) PORTD |= (1 << pin);
					else if (state == LOW) PORTD &= ~(1 << pin);
					else return GPIO_INVALID_PIN_STATE;
				} 
				else return GPIO_INVALID_DIR;
			}
			else return GPIO_INVALID_PIN;
			break;
		default:
			return GPIO_INVALID_PORT;
	}

	return GPIO_OK;
}

gpio_error_t gpio_port_read(gpio_port_t port, uint8_t* result, uint8_t mask) {
	switch(port) {
		case(GPIO_B):
			if (DDRB == (0x00 | ~mask) || DDRB == 0x00) *result = PINB;
			else return GPIO_INVALID_DIR; 
			break;
		case(GPIO_C):
			if (DDRC == ((0x00 | ~mask) >> 1) || DDRC == 0x00)  *result = PINC;
			else return GPIO_INVALID_DIR; 
			break;
		case(GPIO_D):
			if (DDRD == (0x00 | ~mask) || DDRD == 0x00) *result = PIND;
			else return GPIO_INVALID_DIR; 
			break;
		default: 
			return GPIO_INVALID_PORT;
		}

	return GPIO_OK;
}

gpio_error_t gpio_pin_read(gpio_port_t port, uint8_t pin, bit_t* result) {
	// input_pullup mode auto inverts the values read so that theyre consisten
	switch(port) {
		case(GPIO_B):
			if (((DDRB & (1 << pin)) == 0) && (pin < 8)) *result = (PINB & (1 << pin));
			else if (pin >= 8) return GPIO_INVALID_PIN;
			else return GPIO_INVALID_DIR; 
			break;
		case(GPIO_C):
			if (((DDRC & (1 << pin)) == 0) && (pin < 7)) *result = (PINC & (1 << pin));
			else if (pin >= 7) return GPIO_INVALID_PIN;
			else return GPIO_INVALID_DIR; 
			break;
		case(GPIO_D):
			if (((DDRD & (1 << pin)) == 0) && (pin < 8)) *result = (PIND & (1 << pin));
			else if (pin >= 8) return GPIO_INVALID_PIN;
			else return GPIO_INVALID_DIR; 
			break;
		default: 
			return GPIO_INVALID_PORT;
		}

	return GPIO_OK;
}