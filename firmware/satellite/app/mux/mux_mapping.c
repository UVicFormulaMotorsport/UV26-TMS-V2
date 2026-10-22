#include "mux_mapping.h"
#include <stddef.h>

#define MUX_COUNT 3
#define MUX_CHANNEL_COUNT 8
// using 3 8-1 analog muxes (TI SN74HX4581-Q1) per altium schematic

static const uint8_t mux_sensor_map[MUX_COUNT][MUX_CHANNEL_COUNT] = {
    {3, 2, 1, 4, 7, 6, 5, 8},          // MUX01 (PA2)
    {9, 10, 11, 12, 13, 14, 15, 16},  // MUX02 (PA1)
    {18, 19, 20, 17, 21, 22, 23, 0}  // MUX03 (PA3)
};

MuxMappingStatus mux_get_sensor_index(
    uint8_t mux,
    uint8_t channel,
    uint8_t *sensor_index
)
{
    if (sensor_index == NULL ||
        mux >= MUX_COUNT ||
        channel >= MUX_CHANNEL_COUNT)
    {
        return MUX_MAPPING_INVALID_ARGUMENT;
        // checks for valid mux input, input validation
    }

    uint8_t sensor_number = mux_sensor_map[mux][channel];

    if (sensor_number == 0)
    {
        return MUX_MAPPING_UNUSED_CHANNEL;
    }
    // there is one unused channel in our mux. since we have 3 8-1 multiplexers in place, that is a total of 24 channels for 23 cell segments.

    *sensor_index = sensor_number - 1;

    return MUX_MAPPING_OK;
}