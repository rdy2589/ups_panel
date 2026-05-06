#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_timer.h"
#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>

#include "driver_pwm.h"


void enable_drv(void)
{
    gpio_bit_set(GPIOE, GPIO_PIN_13);
}

void disable_drv(void)
{
    gpio_bit_reset(GPIOE, GPIO_PIN_13);
}

void tim1_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    
    rcu_periph_clock_enable(RCU_AF);

    /*configure PB10 (TIMER1 CH2) as alternate function*/
    gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);

    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);


    /*configure PE13 EN_PWM38kHz_MCU*/

    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    
    enable_drv();

    printf("%s()\n", __func__);
    
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tim1_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
   
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 0; //+1
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;//TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3000;     //0.5 of period, due up & down counting
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);



     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, 3158 * 3 / 4);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE); //TIMER_OC_SHADOW_DISABLE


    //timer_interrupt_enable(TIMER0, TIMER_INT_UP);


   // timer_primary_output_config(TIMER1, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);

    printf("%s()\n", __func__);
}
