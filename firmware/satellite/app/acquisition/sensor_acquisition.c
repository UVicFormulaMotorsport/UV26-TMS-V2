#include "sensor_acquisition.h"
#include "mux/mux_mapping.h"
#include <stddef.h>

SensorAcquisitionStatus sensor_read_raw(
    uint8_t mux,
    uint8_t channel,
    uint16_t *adc_value
)
{
    if (adc_value == NULL){
        return ACQUISITION_ERROR;
        // check for null pointer deref
    }

    // local variables
    uint8_t sensor_index;
    MuxMappingStatus sensor_valid = mux_get_sensor_index(mux, channel, &sensor_index);

    if (sensor_valid != MUX_MAPPING_OK){
        return ACQUISITION_ERROR;
    }

    // we will end up here if *adc_value is not null and if the mux-mapped sensor is valid
    // this is where the actual adc read logic should happen
    // remember that when addressing s2s1s0 for the MUX, we are going to recieve 3 analog voltage reads simultaneously

    // the actual GPIO select lines logic is separated from here.

    return ACQUSITION_OK;

}