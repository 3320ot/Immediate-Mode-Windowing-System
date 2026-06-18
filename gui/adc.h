#ifndef adc_h
#define adc_h

#include "ch32v00x.h"
#include "gui.h"

#define ADC_BUFFER_SIZE 128

extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t adc_update_pending;

void ADC_Enable();

#endif
