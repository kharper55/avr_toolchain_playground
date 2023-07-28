/***********************************************************************
* Application for AVR UART, TWI, GPIO peripherals, etc.                *
* @author Kevin Harper                                                 *
* @date July 12, 2023                                                  *
* Purpose: Show how to use the GPIO driver in the applications         *
*          Debugged with UART thru the onboard CH340 IC for the nano   *
*		   which is home to an ATMega328P                              *
***********************************************************************/

#include <avr/io.h>
#include "gpio/gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define F_SCL 400000UL
#define UART_BAUD 115200UL

#include "uart/uart.h"
#include <avr/interrupt.h>
#include "twi/twi_hal.h"
#include "gpio/gpio_types.h"

#include "adc/adc.h"

#include <stdlib.h>  //itoa()

#define RTC_ADDR 0x68 /*Address for DS3231 RTC clock*/

uint8_t numBits = 6;
uint8_t flagCount = 0;
uint8_t i;					// bad name for a modular program
uint8_t maxBitPos = 7;

uint8_t buff[2];

uint8_t print_buffer[24];

uint8_t error;
uint8_t count;

bit_t *c0_val;

uint16_t adc_val;

void main(void) {

	uart_init(TX, false, NONE);

	_delay_ms(10);

	uart_transmit_string((unsigned char*)"UART configured as output @ 115200 baud");
	uart_transmit_nl(1, false);

	//adc_init(INTERNAL_VREF, FREE, ADC2/*PC6 alt fxn*/, false);

	//ADCSRB |= ();                       //Set trigger source
	DDRC &= ~(1 << PINC3);
    ADMUX = 0x43;            //Select a mux channel to start and select voltage reference
    //ADMUX &= ~(1 << ADLAR);                 //Ensure bit ordering is consistent with the adc_read fxn
    DIDR0 &= ~(1 << ADC3);                   //Disable digital input buffer
    //Enable the ADC functionality, enable auto triggering by HW, set start conversion bit, and
    // set prescalar
    //ADCSRA |= ADCPSC_VAL;  // set PSC value so sample rate b/w 50kHz -- 200kHz for 10bit res
    //ADCSRA |= ((1 << ADEN) /*| (1 << ADATE) | (1 << ADSC)*/);
	ADCSRA = 0x87;

	uart_transmit_string((unsigned char*)"ADC initialized to read from PC6 with 1.1V internal reference");
	uart_transmit_nl(1, false);

	// mask == 0xFF, include all 
	// mask == 0xDF, omit bit 5
	error = gpio_port_init(GPIO_B, DIR_OUTPUT, 0xFF);

	_delay_ms(10);

	if (error == GPIO_OK) {
		uart_transmit_string((unsigned char*)"PORTB configured as output");
		uart_transmit_nl(1, false);
	}

	for (count = 2; count <= 7; count++) {
		error = gpio_pin_init(GPIO_D, DIR_OUTPUT, count);
	}

	_delay_ms(10);
    
	if (error == GPIO_OK) {
		uart_transmit_string((unsigned char*)"PORTD bits 2:7 configured as output");
		uart_transmit_nl(1, false);
	}

	// initialization and reads are not workign with input pullup selected
	error = gpio_pin_init(GPIO_C, DIR_INPUT_PULLUP, PINC0);

	_delay_ms(10);

	if (error == GPIO_OK) {
		uart_transmit_string((unsigned char*)"PORTC bit 0 configured as input pullup");
		uart_transmit_nl(1, false);
	}

	else {
		uart_transmit_string((unsigned char*)"Error initializing GPIO input...");
		uart_transmit_byte(error + 0x30);
		uart_transmit_nl(1, false);
	}

	twi_init(F_SCL, false);

	_delay_ms(10);
	
	uart_transmit_string((unsigned char*)"TWI bit rate and SCL initialized");
	uart_transmit_nl(1, false);

	// BCD encoded, so hex values == decimal
	uint8_t rtc_data[7] = {0x50, 0x46, 0x20, 0x07, 0x16, 0x07, 0x23};

	sei(); /*Enable interrupts, necessary for I2C*/

	uart_transmit_string((unsigned char*)"Taking control of and writing to I2C bus");
	uart_transmit_nl(1, false);
	
	uint8_t err = twi_write(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
	if(err != TWI_OK) {
		memset(print_buffer,0,sizeof(print_buffer));
		sprintf(print_buffer,"%d error %d\r\n\n",__LINE__,err);
		uart_transmit_string((uint8_t*)print_buffer);
		//while(1);
	}

	DDRB |= (1 << DDB1);
   	// PB1 as output
   	OCR1A = 0x01FF;
   	// set PWM for 50% duty cycle at 10bit
   	TCCR1A |= (1 << COM1A1);
   	// set non-inverting mode
   	TCCR1A |= (1 << WGM11) | (1 << WGM10);
   	// set 10bit phase corrected PWM Mode
   	TCCR1B |= (1 << CS11);
   	// set prescaler to 8 and starts PWM
    
	_delay_ms(1000);

	uart_transmit_string((unsigned char*)"Initialization complete.");
	uart_transmit_nl(2, false);

	while(1) {
		
		err = twi_read(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
		if(err != TWI_OK){
			memset(print_buffer,0,sizeof(print_buffer));
			sprintf(print_buffer,"%d error %d\r\n\n",__LINE__,err);
			uart_transmit_string((uint8_t*)print_buffer);
			//while(1);
		}
		else {
			memset(print_buffer,0,sizeof(print_buffer));
			sprintf(print_buffer,"\r20%02x/%02x/%02x %02x:%02x:%02x",
				rtc_data[6],
				rtc_data[5],
				rtc_data[4],
				rtc_data[2],
				rtc_data[1],
				rtc_data[0]
				);
			uart_transmit_string((unsigned char*)print_buffer);
			uart_transmit_nl(2, false);

		//adc_read(ADC3, *adc_val);
		}

		ADCSRA |= (1 << ADSC);  
    	while((ADCSRA & (1 << ADIF)) == 0);
    	_delay_us(10);
    	// the below line will change according the the ADLAR bit
    	adc_val = (ADCL | ((ADCH & 0x03) << 8))&(0x3FF);
		//*adc_val |= ADCL;
		//*adc_val |= ((ADCH & 0x03) << 8);
		//*adc_val &= (0x3FF);

		uart_transmit_string((unsigned char*)itoa(adc_val, print_buffer, 10));
		uart_transmit_nl(2, false);

		uart_transmit_string((unsigned char*)"Flashing bits 0:5 on port B...");
		uart_transmit_nl(2, false);

		// Dont flash pinB6 or 7; external oscillator pins
		for (i = 0; i <= maxBitPos - 2; i++) {
			if(i!=1) {
				gpio_pin_write(GPIO_B, i, HIGH);
				_delay_ms(100);
				gpio_pin_write(GPIO_B, i, LOW);
				_delay_ms(100);
				uart_transmit_byte(i + 0x30);
				uart_transmit_string((unsigned char*)" done.");
				uart_transmit_nl(1, false);
			}
		}

		uart_transmit_nl(1, false);
		_delay_ms(100);

		uart_transmit_string((unsigned char*)"Flashing bits 2:7 on port D...");
		uart_transmit_nl(2, false);

		// Dont flash pinB6 or 7; external oscillator pins
		for (i = 2; i <= maxBitPos; i++) {
			gpio_pin_write(GPIO_D, i, HIGH);
			_delay_ms(100);
			gpio_pin_write(GPIO_D, i, LOW);
			_delay_ms(100);
			uart_transmit_byte(i + 0x30);
			uart_transmit_string((unsigned char*)" done.");
			uart_transmit_nl(1, false);
		}
		
		uart_transmit_nl(1, false);
		_delay_ms(100);

		uart_transmit_string((unsigned char*)"Writing bits 2:7 on port D at once...");
		uart_transmit_nl(2, false);

		error = 0;
		error = gpio_port_write(GPIO_D, 0xFF, 0xFC);
		if (error != GPIO_OK) {
			uart_transmit_string((unsigned char*)"Error ");
			uart_transmit_byte(0x30 + error);
			uart_transmit_nl(2, false);
		}
		_delay_ms(250);
		gpio_port_write(GPIO_D, 0x00, 0xFC);

		uart_transmit_string((unsigned char*)"Reading PINC bit 0");
		uart_transmit_nl(2, false);

		error = 0;
		error = gpio_pin_read(GPIO_C, PINC0, c0_val);

		if (*c0_val == 1 && error == GPIO_OK) {
			uart_transmit_string((unsigned char*)"Pin C0 is pulled up!");
			uart_transmit_nl(2, false);
		}
		else if (*c0_val == 0 && error == GPIO_OK) {
			uart_transmit_string((unsigned char*)"Pin C0 is pulled down... :(");
			uart_transmit_nl(2, false);
		}
		else {
			uart_transmit_string((unsigned char*)"GPIO read error ");
			uart_transmit_byte(error + 0x30);
			uart_transmit_nl(2, false);
		}
		
		uart_transmit_string((unsigned char*)"All GPIO tests done.");
		uart_transmit_nl(2, false);
		_delay_ms(100);

		uart_transmit_string((unsigned char*)"Starting UART Flag Check...");
		uart_transmit_nl(2, false);

		for (i = maxBitPos; i >= maxBitPos - numBits; i--) {
			// Dont increment for transmit complete or UDRE
			if ((uart_check_flag(i) == true) && (!(i == TXC0) || (i == UDRE0))) flagCount++;
		}

		if (flagCount == 0) {
			uart_transmit_string((unsigned char*)"No flags found.");
			uart_transmit_nl(2, false);
		}
		else {
			decToASCII(buff, flagCount);
			uart_transmit_string((unsigned char*)buff);
			uart_transmit_string((unsigned char*)" flags found.");
			uart_transmit_nl(2, false);
		}

		_delay_ms(2000);
	}
}

/* =================================================================================
 *                                    // END FILE //
 * ================================================================================= */