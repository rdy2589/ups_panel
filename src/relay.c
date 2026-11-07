#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"

#include "relay.h"
#include <stdio.h>


uint16_t sound_timer = 0;

void sound_(uint16_t time_ms)
{
    //static uint16_t beep_time_ms = 0;

    if(time_ms && !sound_timer)
    {
        sound_timer = time_ms;
    }
}

void sound_proceed(void)
{
    if(!sound_timer)
    {
        gpio_bit_reset(GPIOB, GPIO_PIN_14);
    }
    else
    {
        sound_timer--;
        gpio_bit_set(GPIOB, GPIO_PIN_14);
    }
}


void overheat_relay_enable(void)
{
    gpio_bit_set(RELAY_gpio, RELAY_OVH_pin);
}

void overheat_relay_disable(void)
{
    gpio_bit_reset(RELAY_gpio, RELAY_OVH_pin);
}

void battery_relay_enable(void)
{
    gpio_bit_set(RELAY_gpio, RELAY_BAT_pin);
}

void battery_relay_disable(void)
{
    gpio_bit_reset(RELAY_gpio, RELAY_BAT_pin);
}

void mlf_relay_enable(void)
{
    gpio_bit_set(RELAY_gpio, RELAY_MLF_pin);
}

void mlf_relay_disable(void)
{
    gpio_bit_reset(RELAY_gpio, RELAY_MLF_pin);
}

void led_ovh_enable(void)
{
    gpio_bit_set(LED_gpio, LED_OVH_pin);
}

void led_ovh_disable(void)
{
    gpio_bit_reset(LED_gpio, LED_OVH_pin);
}

void led_bat_enable(void)
{
    gpio_bit_set(LED_gpio, LED_BAT_pin);
}

void led_bat_disable(void)
{
    gpio_bit_reset(LED_gpio, LED_BAT_pin);
}

void led_line_enable(void)
{
    gpio_bit_set(LED_gpio, LED_LINE_pin);
}

void led_line_disable(void)
{
    gpio_bit_reset(LED_gpio, LED_LINE_pin);
}

void led_bat_mlf_enable(void)
{
    gpio_bit_set(LED_gpio, LED_BAT_MLF_pin);
}

void led_bat_mlf_disable(void)
{
    gpio_bit_reset(LED_gpio, LED_BAT_MLF_pin);
}





void relay_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(RELAY_gpio, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RELAY_OVH_pin | RELAY_BAT_pin | RELAY_MLF_pin);
    overheat_relay_enable();
    battery_relay_enable();
    mlf_relay_enable();
    printf("%s()\n", __func__);
}

void led_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_init(LED_gpio, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_OVH_pin | LED_BAT_pin | LED_LINE_pin | LED_BAT_MLF_pin);
    printf("%s()\n", __func__);
}


void sound_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    printf("%s()\n", __func__);
    sound_(0);
}