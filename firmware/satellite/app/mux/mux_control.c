#include "mux_control.h"
#include "gpio.h"
#include <stddef.h>

// static is the private keyword
static void mux_write_gpio(
    uint8_t s2,
    uint8_t s1,
    uint8_t s0){
    // private helper function to perform HAL GPIO functions
    // takes in 3 set bits, which form a 3 bit binary number
    // endianness?? we assume that the rightmost bit is the MSB, but is this correct?
    // put actual pin masks there
    (s2 == 1 ? HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
    (s1 == 1 ? HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
    (s0 == 1 ? HAL_GPIO_WritePin(GPIOB, gpio_pin_mask, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_port, gpio_pin_mask, GPIO_PIN_RESET))
     // write a binary number 0-7 to the mux select pins to address the sensors
   
}
// you might be confused on where s2 s1 s0 come from. this is a helper function for the one below

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
    // here the mux is written. so we should have values come in the "data in" lines in the IOC. 
    // keep in mind, we are reading an analog voltage from those lines
    // those lines are 
}

