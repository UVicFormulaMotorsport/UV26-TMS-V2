#ifndef MUX_CONTROL_H
#define MUX_CONTROL_H

#include <stdint.h>

typedef enum 
{
    MUX_CONTROL_OK,
    MUX_CONTROL_ERROR
} MuxControlStatus;

MuxControlStatus mux_select_channel(
    uint8_t channel
);

#endif

