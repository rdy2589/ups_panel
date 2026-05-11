#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"

#include "relay.h"
#include <stdio.h>




void sound_(uint16_t state)
{
    if(state)
        gpio_bit_set(GPIOB, GPIO_PIN_14);
    else
        gpio_bit_reset(GPIOB, GPIO_PIN_14);
}




void inpit_relay_enable(void)
{
    gpio_bit_set(GPIOB, GPIO_PIN_13);
}

void input_relay_disable(void)
{
    gpio_bit_reset(GPIOB, GPIO_PIN_13);
}

void charge_relay_enable(void)
{
    gpio_bit_set(GPIOB, GPIO_PIN_12);
}

void charge_relay_disable(void)
{
    gpio_bit_reset(GPIOB, GPIO_PIN_12);
}

void relay_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
   // gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13);
    printf("%s()\n", __func__);
}

void sound_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    printf("%s()\n", __func__);
    sound_(0);
}