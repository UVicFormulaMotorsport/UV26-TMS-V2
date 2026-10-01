#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stdint.h>

// public types
typedef enum
{
    TEMPERATURE_STATUS_OK,
    TEMPERATURE_STATUS_INVALID_VOLTAGE
} TemperatureStatus;

// public functions
float temperature_from_adc(uint16_t adc_value);

TemperatureStatus temperature_voltage_to_celsius(float voltage, float *temperature_c);

#endif