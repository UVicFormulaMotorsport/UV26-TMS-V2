#include "mux_control.h"
#include "gpio.h"
#include <stddef.h>

static void mux_write_gpio(
    uint8_t s2,
    uint8_t s1,
    uint8_t s0){
    // private helper function to perform HAL GPIO functions
    (s2 == 1 ? HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
    (s1 == 1 ? HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
    (s0 == 1 ? HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
    
    // write a binary number 0-7 to the mux select pins to address the sensors
    HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, s2 = 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, s1 = 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, s0 = 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

MuxControlStatus mux_select_channel(
    uint8_t channel
){
    if (channel > 7){
        return MUX_CONTROL_ERROR;
    }
    // there is no need to check channel < 0 as channel is unsigned
    // if we end up here, the channel selection is valid

    mask = 0x01; // 00000001
    s2 = channel & mask; // mask first bit
    s1 = (channel >> 1) & mask; // mask second bit
    s0 = (channel >> 2) & mask; // mask third bit
    // ">>" is the LOGICAL SHIFT operator
    // bitmasking operations to extract the select values for the provided channel
    // we need to send a 3 bit binary number over gpio to read the adc channel through the mux
    mux_write_gpio(s2,s1,s0);
}

