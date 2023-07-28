
#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

typedef enum adc_error {
    ADC_OK,
    ADC_INVALID_MUX,
    ADC_INVALID_TRIG,
    ADC_INVALID_REF
} adc_error_t;

typedef enum voltage_reference {
    EXTERNAL_VREF = 0,
    ANALOG_VCC = 1,
    INTERNAL_VREF = 3
} voltage_ref_t;

typedef enum mux_value {
    /*Note:*/
    // Channels 5:0 have digital input buffer
    // needing to be disabled by writing a one to
    // the corresponding bit in the DIDR0 register.
    // The corresponding PIN register bit will always 
    // read as zero when this bit is set.
    // This is a power reduction measure.
    ADC0,
    ADC1,
    ADC2,
    ADC3,
    ADC4,
    ADC5,
    ADC6,
    ADC7, 
    ADC8 /*Temperature sensor*/
} mux_value_t;

// ADCSRB bits 2:0 
typedef enum trigger_source {
    FREE,   /*Free running mode*/
    AIN,    /*Analog comparator*/
    EXTI0,  /*External interupt request 0*/
    TC0CMA, /*Timer/counter0 compare match A*/
    TC0OV,  /*Timer/counter0 overflow*/
    TC1CMB, /*Timer,counter1 compare match B*/
    TC1OV,  /*Timer/counter1 overflow*/
    TC1CE   /*Timer/capture1 capture event*/
} trigger_source_t;

/*By default, the successive approximation circuitry requires an input clock frequency between 50kHz and 200kHz to get
maximum resolution. If a lower resolution than 10 bits is needed, the input clock frequency to the ADC can be higher than
200kHz to get a higher sample rate*/

#define ADCPSC_VAL 7

//16MHz / 128 == 125kHz sample rate, write 7 to the ADPS bits 2:0

adc_error_t adc_init(voltage_ref_t vref, trigger_source_t trig, mux_value_t mux, bool intEn);
adc_error_t adc_read(mux_value_t mux, uint16_t* value);

#endif //ADC_H_