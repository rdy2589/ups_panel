#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_timer.h"

#include <stdio.h>
#include <stdint.h>

#include "encoder.h"

#define ENC_TIMER_RELOAD        0xFFFFU
#define ENC_TIMER_INPUT_FILTER  0x06U//0x03U

void enc_init(void)
{
    timer_parameter_struct timer_init_struct;
    timer_ic_parameter_struct timer_ic_init;

    printf("%s()\n", __func__);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(ENC_TIMER_RCU);

    /* TIMER2 CH0/CH1 default mapping: PA6 / PA7. */
    gpio_init(ENC_gpio, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,
              ENCA_pin | ENCB_pin | ENCSW_pin);

    timer_deinit(ENC_TIMER);

    timer_struct_para_init(&timer_init_struct);
    timer_init_struct.prescaler = 0U;
    timer_init_struct.alignedmode = TIMER_COUNTER_EDGE;
    timer_init_struct.counterdirection = TIMER_COUNTER_UP;
    timer_init_struct.period = ENC_TIMER_RELOAD;
    timer_init_struct.clockdivision = TIMER_CKDIV_DIV1;
    timer_init_struct.repetitioncounter = 0U;
    timer_init(ENC_TIMER, &timer_init_struct);

    timer_quadrature_decoder_mode_config(ENC_TIMER,
                                         TIMER_ENCODER_MODE2,
                                         TIMER_IC_POLARITY_RISING,
                                         TIMER_IC_POLARITY_RISING);

    timer_channel_input_struct_para_init(&timer_ic_init);
    timer_ic_init.icpolarity = TIMER_IC_POLARITY_RISING;
    timer_ic_init.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_ic_init.icprescaler = TIMER_IC_PSC_DIV1;
    timer_ic_init.icfilter = ENC_TIMER_INPUT_FILTER;
    timer_input_capture_config(ENC_TIMER, TIMER_CH_0, &timer_ic_init);
    timer_input_capture_config(ENC_TIMER, TIMER_CH_1, &timer_ic_init);

    timer_counter_value_config(ENC_TIMER, 0U);
    timer_enable(ENC_TIMER);
}

int16_t enc_get_position(void)
{
    return (int16_t)(uint16_t)timer_counter_read(ENC_TIMER);
}
