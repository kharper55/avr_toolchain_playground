/*
 * twi_hal.h
 *
 * Created: 2020/11/01 18:07:26
 *  Author: DevilBinder
 */ 

#ifndef TWI_HAL_H_
#define TWI_HAL_H_

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#define TWI_TIMEOUT 1600

#define TWI_START		0x08
#define TWI_RSTART		0x10

// Master transmitter status codes
#define TWIT_ADDR_ACK	0x18
#define TWIT_ADDR_NACK	0x20
#define TWIT_DATA_ACK	0x28
#define TWIT_DATA_NACK	0x30

// Master receiver status codes
#define TWIR_ADDR_ACK	0x40
#define TWIR_ADDR_NACK	0x48
#define TWIR_DATA_ACK	0x50
#define TWIR_DATA_NACK	0x58

#define TWI_ERROR		0x38
#define TWI_NONE		0xF8

typedef enum {
	TWI_OK,
	TWI_ERROR_START,
	TWI_ERROR_RSTART,
	TWI_NACK,
	TWI_PUD
} twi_error_t;


twi_error_t twi_init(uint32_t speed, bool PUE);
twi_error_t twi_write(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t len);
twi_error_t twi_read(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t len);
/*all other functions are static and therefore not delcared here*/

#endif /* TWI_HAL_H_ */