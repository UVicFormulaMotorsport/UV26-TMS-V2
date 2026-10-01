#include <assert.h>
#include <math.h>

#include "temperature.h"

int main(void){

    float temperature_c;
    
    TemperatureStatus status = temperature_voltage_to_celsius(1.83f, &temperature_c);

    assert(status == TEMPERATURE_STATUS_OK);
    assert(fabsf(temperature_c - 27.5f) < 0.001f);

    return 0;
}