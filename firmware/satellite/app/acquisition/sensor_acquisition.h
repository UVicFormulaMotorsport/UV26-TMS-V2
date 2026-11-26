#ifndef SENSOR_ACQUISITION_H
#define SENSOR_ACQUISITION_H

#include <stdint.h>

typedef enum {
    ACQUISITION_OK,
    ACQUISITION_ERROR
} SensorAcquisitionStatus;

// mux_get_sensor_index() tells us which sensor we are reading
// sensor_read_raw() will tell us the raw ADC value being read at that sensor 

SensorAcquisitionStatus sensor_read_raw(
    // function inputs
    uint8_t mux,
    uint8_t channel,
    uint16_t *adc_value
);

#endif