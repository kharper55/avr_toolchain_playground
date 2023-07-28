/***********************************************************************
* Hardware Abstraction for AVR Microcontrollers UART driver            *
* @author Kevin Harper                                                 *
* @date July 13, 2023                                                  *
* Purpose: Provide an easy interface to deal with serial terminal      *
***********************************************************************/

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdbool.h>

#define round_(x) (x - (unsigned long)(x)>=0?(unsigned long)((x)+1):(unsigned long)((x)-1))
#define UBRR_VALUE (((F_CPU / (16UL * (UART_BAUD))) - 1UL))
#define UBRR_VALUE_ROUNDED_NEAREST round_(UBRR_VALUE) //round is included in utility.h
#define UBRRH_VALUE ((uint8_t)UBRR_VALUE_ROUNDED_NEAREST >> 8)
#define UBRRL_VALUE ((uint8_t)UBRR_VALUE_ROUNDED_NEAREST)

typedef enum {
    TX, 
    RX, 
    BOTH
} uart_dir_t;

typedef enum {
    NONE = 0, 
    EVEN = 2, 
    ODD = 3
} uart_parity_t;

// Change return types to an error type in lieu of "void"...?

void uart_init(uart_dir_t dir, bool int_en, uart_parity_t par);
bool uart_check_flag(uint8_t flag);         /*to be used to check flags
                                                without need for using 
                                                register access inlines*/
void decToASCII(uint8_t buffer[], uint8_t decimal);
uint8_t uart_read_byte(); //stub for future fxn
void uart_read_string(uint8_t buffer[], uint8_t len);  //stub for future fxn
void uart_transmit_byte(unsigned char data);
void uart_transmit_string(unsigned char* str);
void uart_transmit_nl(int num, bool cr);

#endif //UART_H_