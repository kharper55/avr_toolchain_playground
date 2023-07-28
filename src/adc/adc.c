
#include "adc.h"

adc_error_t adc_init(voltage_ref_t vref, trigger_source_t trig, mux_value_t mux, bool intEn) {
    ADCSRB |= (trig);                       /*Set trigger source*/
    ADMUX = (mux) | (vref << 6);            /*Select a mux channel to start and select voltage reference*/
    ADMUX &= ~(1 << ADLAR);                 /*Ensure bit ordering is consistent with the adc_read fxn*/
    if (mux < 6) DIDR0 &= ~(1 << mux);      /*Disable digital input buffer*/
    if (intEn) ADCSRA |= (1 << ADIE);
    /*Enable the ADC functionality, enable auto triggering by HW, set start conversion bit, and
     set prescalar*/
    ADCSRA |= ADCPSC_VAL; /* set PSC value so sample rate b/w 50kHz -- 200kHz for 10bit res*/
    ADCSRA |= ((1 << ADEN) | (1 << ADATE) | (1 << ADSC));  
    return ADC_OK;
}

adc_error_t adc_read(mux_value_t mux, uint16_t* value) {
    //ADMUX |= mux;
    ADCSRA |= (1 << ADSC);  
    while(!(ADCSRA & (1 << ADIF)));
    _delay_us(10);
    // the below line will change according the the ADLAR bit
    *value = (uint16_t)(ADCL | ((ADCH & 0x03) << 8))&(0x3FF);
    // below line needed if free running/auto triggering is not enabled
    //ADCSRA |= (1 << (ADSC));
    //if (!(ADCSRA & (1 << ADIE) >> ADIE)) (ADCSRA |= 1 << ADIF);
    return ADC_OK;
}