#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>

typedef enum
{
    ADC_READ_OK,
    ADC_READ_ERROR
} ADCReadStatus;

// function to read adc
// needs to know which of the 3 analog inputs to read
// and where to store the measurement
ADCReadStatus read_adc(
    uint8_t adc_input,
    uint16_t *adc_value
);

#endif