#include "adc_driver.h"
#include "adc.h"
#include <stddef.h>

#define ADC_INPUT_LINE_COUNT 3
static const uint32_t adc_sensor_lookup[ADC_INPUT_LINE_COUNT] = {ADC_CHANNEL_2, ADC_CHANNEL_1, ADC_CHANNEL_3};
// yes, for some reason these are not numbered accordingly in the ioc file
ADCReadStatus read_adc(
    uint8_t adc_input_line,
    uint16_t *adc_value
){
    //check if input parameters are valud
    if (adc_input_line >= ADC_INPUT_LINE_COUNT  || adc_value == NULL){
        return ADC_READ_ERROR;
        // invalid adc input line supplied or adc_value points to null
    }

    // input line 0 = MUX01 on PA2
    // input line 1 = MUX02 on PA1
    // input line 2 = MUX03 on PA3

    // configure adc
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = adc_sensor_lookup;// this needs to dynamically change

    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        return ADC_CONFIG_ERROR;
    }


}