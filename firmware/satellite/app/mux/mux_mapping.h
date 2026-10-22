#ifndef MUX_MAPPING_H
#define MUX_MAPPING_H

#include <stdint.h>

typedef enum
{
    MUX_MAPPING_OK = 0,
    MUX_MAPPING_UNUSED_CHANNEL,
    MUX_MAPPING_INVALID_ARGUMENT

} MuxMappingStatus;

MuxMappingStatus mux_get_sensor_index(
    uint8_t mux,
    uint8_t channel,
    uint8_t *sensor_index
);

#endif //MUX_MAPPING_H
