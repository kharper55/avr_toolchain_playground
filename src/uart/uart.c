/***********************************************************************
* Hardware Abstraction for AVR Microcontrollers UART driver            *
* @author Kevin Harper                                                 *
* @date July 13, 2023                                                  *
* Purpose: Provide an easy interface to deal with serial terminal      *
*                                                                      *
* The Arduino Nano board feat. the ATMeg328P has a CH340 bidirectional *
* Serial-USB chip providing an interface to the serial terminal on PC  *
* through the board mounted micro-usb header for debugging.            *
***********************************************************************/

#include <avr/io.h>
#include "uart.h"

// Need for BR calculation inside uart.h
#define UART_BAUD 115200UL

/************************* UART Utility Stuff *************************/

// This forces asynchronous mode on USART0 with 8-bit data width, 1 stop bit
void uart_init(uart_dir_t dir, bool int_en, uart_parity_t par) {
    // Setup baudrate with integer calculated in uart.h macros
    //hw_reg8_write(UBRR0H, UBRRH_VALUE);
    //hw_reg8_write(UBRR0L, UBRRL_VALUE);

    // Calculated using eqn from datasheet and rounded to nearest int
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // Set UART direction
    switch (dir) {
        case(TX):
            //hw_bit_write(USCR0B, TXEN0, 1); 
            UCSR0B |= (1 << TXEN0);
            break;
        case(RX):
            //hw_bit_write(USCR0B, RXEN0, 1); 
            UCSR0B |= (1 << RXEN0);
            break;
        case(BOTH):
            //hw_bit_write(USCR0B, TXEN0, 1); 
            //hw_bit_write(USCR0B, RXEN0, 1); 
            UCSR0B |= (1 << TXEN0);
            UCSR0B |= (1 << RXEN0);
            break;
        //default: return ERROR?
    }
    // Optionally enable interrupts
    // This requires setting up the vector table and assigning space to store the ISR
    if (int_en) {
        if ((dir == TX) || BOTH) UCSR0B |= (1 << TXCIE0);
        else if ((dir == RX) || BOTH) UCSR0B |= (1 << RXCIE0);
        //hw_bit_write(USCR0B, UDRIE0, 1);
        UCSR0B |= (1 << UDRIE0);
    }
    // Set 8-bit data width, asynchronous mode, 1 stop-bit,
    // with user input parity
    //hw_reg8_write(USCR0C, (0x06 | (par << 4)));
    UCSR0C = (0x06 | (par << 4));
}

bool uart_check_flag(uint8_t flag) {
    static bool return_val;
    switch(flag) {
        /* Check receive complete flag, bit 7 */
        case(RXC0):
            return_val = (UCSR0A & (1 << RXC0))?true:false;
            break;
        /* Check transmit complete flag, bit 6 */
        case(TXC0):
            return_val = (UCSR0A & (1 << TXC0))?true:false;
            break;
        /* Check UART data register empty flag, bit 5 */
        case(UDRE0):
            return_val = (UCSR0A & (1 << UDRE0))?true:false;
            break;
        /* Check frame error flag, bit 4 */
        case(FE0):
            return_val = (UCSR0A & (1 << FE0))?true:false;
            break;
        /* Check data overrun flag, bit 3 */
        case(DOR0):
            return_val = (UCSR0A & (1 << DOR0))?true:false;
            break;
        /* Check parity error flag, bit 2 */
        case(UPE0):
            return_val = (UCSR0A & (1 << UPE0))?true:false;
            break;
    }
    return return_val; 
}

// Rewrite this to be used in a loop that increments in base 10's to extract each digit 

// Sub function to convert a decimal represented as an unsigned integer 
// into it's equivalent ASCII character string
void decToASCII(uint8_t buffer[], uint8_t decimal) {
     //If the number to be converted is <= 2 digits wide, we can convert it 
     //(ie 0 -- 99; this is limited by the buffer array and requires deeper logic for larger integers)
     if (!(decimal >= 100) || sizeof(buffer)/sizeof(buffer[0]) > 2) {
          //Check if the number to be converted is smaller than 10
          if (decimal / 10 == 0) {
             //Set ones place equal to remainder of integer division of decimal / 10
             buffer[0] = decimal % 10 + '0';
             buffer[1] = '\0';
          }
          //The number to be converted is between 0 and 99
          else {
             //Set tens place equal to integer division of decimal / 10
             buffer[0] = decimal / 10 + '0';
             //Set ones place equal to remainder of integer division of decimal / 10
             buffer[1] = decimal % 10 + '0';
          }
     }
}

/************************ UART Receive Stuff **************************/

uint8_t uart_read_byte() {
    return 0;
}
// Does a function like this even make sense?
void uart_read_string(uint8_t buffer[], uint8_t len) {}

/************************ UART Transmit Stuff *************************/

//  Direct from ATMega328P datasheet pg 150
void uart_transmit_byte(unsigned char data) {
    /* Wait for empty transmit buffer */
    //while (!(hw_reg8_read(USCR0A) & (1 << UDRE0)));
    while (!(UCSR0A & (1 << UDRE0)));
    /* Put data into buffer, sends the data */
    //hw_reg8_write(UDR0, data);
    UDR0 = data;
}

void uart_transmit_string(unsigned char* str) {
    int i = 0;
    // While there are still bytes to transmit...
    while (str[i] != '\0') {
        // clear transmit complete flag
        uart_transmit_byte(str[i]);
        i++;
    }
}

void uart_transmit_nl(int num, bool cr) {
    for (int i = 0; i < num; i++) {
        uart_transmit_byte('\n');
    }
    if (cr) uart_transmit_byte('\r');
}