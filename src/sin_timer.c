#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_timer.h"
#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>

#include "sin_timer.h"
#include "sin_table.h"



void enable_bridge(void)
{
    gpio_bit_set(GPIOE, GPIO_PIN_14);
}

void disable_bridge(void)
{
    gpio_bit_reset(GPIOE, GPIO_PIN_14);
}


void tim0_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOE);
    //rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*configure PB13 PB14 PB15(TIMER0 CH0N CH1N CH2N) as alternate function*/
    gpio_pin_remap_config(GPIO_TIMER0_FULL_REMAP, ENABLE);

    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

     /*configure PE14 DIS_PWM_MCU*/
    enable_bridge();
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tim0_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);
   
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 0; //+1
    timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_BOTH;//TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = PWM_MID;     //0.5 of period, due up & down counting
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);


    timer_break_struct_para_init(&timer_breakpara);
    // timer_brakpara.runoffstate      = TIMER_ROS_STATE_ENABLE;//,TIMER_ROS_STATE_DISABLE
    // timer_brakpara.ideloffstate     = TIMER_IOS_STATE_ENABLE;//,TIMER_IOS_STATE_DISABLE
    timer_breakpara.deadtime         = 20;
    // timer_brakpara.breakpolarity    = TIMER_BREAK_POLARITY_LOW;//,TIMER_BREAK_POLARITY_HIGH
    // timer_brakpara.outputautostate  = TIMER_OUTAUTO_ENABLE;//,TIMER_OUTAUTO_DISABLE
    // timer_brakpara.protectmode      = TIMER_CCHP_PROT_OFF;//,TIMER_CCHP_PROT_0,TIMER_CCHP_PROT_1,TIMER_CCHP_PROT_2
    // timer_brakpara.breakstate       = TIMER_BREAK_DISABLE;
    timer_break_config(TIMER0, &timer_breakpara);

     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER0,TIMER_CH_1,&timer_ocintpara);
    //timer_channel_output_config(TIMER0,TIMER_CH_2,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,100);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_ENABLE); //TIMER_OC_SHADOW_DISABLE

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,200);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_1,TIMER_OC_SHADOW_ENABLE); //TIMER_OC_SHADOW_DISABLE

    //timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,200);
    //timer_channel_output_mode_config(TIMER0,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    //timer_channel_output_shadow_config(TIMER0,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_interrupt_enable(TIMER0, TIMER_INT_UP);


    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);
}
