#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
//#include "gd32f30x_timer.h"
#include "gd32f30x_adc.h"

#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>

#include "sin_timer.h"
#include "adc.h"
#include "systick.h"
#include "global.h"
#include "sin_table.h"

uint16_t inserted_data[4];

void zero_detect_config(void)
{
    printf("%s()\n", __func__);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_AF);
    
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    //EXTI7
    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_7);

    /* configure key EXTI line */
    exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(EXTI_7);
}

// TIMER6
void period_timer_config()
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;

    rcu_periph_clock_enable(RCU_TIMER6);

    timer_deinit(TIMER6);
   
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 0xFFFF;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER6,&timer_initpara);

    timer_interrupt_enable(TIMER0, TIMER_INT_UP);

/*
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.deadtime         = 20;
    timer_break_config(TIMER0, &timer_breakpara);
*/

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER6);
}
void period_timer_stop()
{
    timer_disable(TIMER6);
}
void period_timer_start()
{
    TIMER_CNT(TIMER6) = 0;
    timer_enable(TIMER6);
}
uint16_t period_timer_get()
{
    return TIMER_CNT(TIMER6);
}



void adc_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    
    rcu_periph_clock_enable(RCU_AF);

    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_ADC1);
    rcu_periph_clock_enable(RCU_ADC2);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);


    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);  


    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, I_OUT_gpio| REF_2_5_gpio | I_PM_gpio | U_BAT_gpio);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, AC_IN_gpio| AC_OUT_gpio | TEMP_gpio | T_BAT_gpio | DU_gpio | DU1_gpio);
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, AC_IN_gpio| AC_OUT_gpio | TEMP_gpio | T_BAT_gpio | DU_gpio | DU1_gpio);

    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1U);
    adc_channel_length_config(ADC2, ADC_REGULAR_CHANNEL, 1U);
    
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC2, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    /* ADC external trigger config */
    adc_external_trigger_config(ADC2, ADC_REGULAR_CHANNEL, ENABLE);    
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);



    /* enable ADC interface */
    adc_enable(ADC0);
    Delay_Ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    printf("%s() ADC0 calibration completed\n", __func__);
    /* enable ADC interface */
    adc_enable(ADC1);
    Delay_Ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
    printf("%s() ADC1 calibration completed\n", __func__);
    /* enable ADC interface */
    adc_enable(ADC2);
    Delay_Ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);
    printf("%s() ADC2 calibration completed\n", __func__);

    /* ADC DMA function enable */
    //adc_dma_mode_enable(ADC0);


    I_out_offset = current_offset();
    U_out_offset = u_out_offset();      // ADC descretes
    U_input_offset = u_input_offset();
    ref_2_5();

    //calculate zero values
    I_zero = hex_to_volt(I_out_offset);// * 0.661;      //volts
    U_zero = hex_to_volt(U_out_offset);// * 0.661;      //volts
    printf("U_zero = %.2f\n", U_zero);

    #ifdef __VFP_FP__
        printf("__VFP_FP__ defined\n");
    #endif

    #ifdef __FPU_USED
        printf("__FPU_USED defined, __FPU_PRESENT = %d\n", __FPU_PRESENT);
    #endif


    //change work mode
    adc_disable(ADC0);
    adc_disable(ADC1);
    adc_disable(ADC2);
    /* ADC DMA function enable */
    adc_dma_mode_disable(ADC0);
    
    //ADC2 - injected channels
    //ADC0 - regular channels
    adc_dma_config();

    adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC2, ADC_CONTINUOUS_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC2, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE); //ADC0_1_EXTTRIG_REGULAR_T1_CH1);
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC2, ADC_INSERTED_CHANNEL, ENABLE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC data alignment config */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC channel length config */
    adc_channel_length_config(ADC2, ADC_INSERTED_CHANNEL, 4);  // I_OUT_channel AC_OUT_channel AC_IN_channel REF_2_5_channel
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL,  4);
  

    /* ADC0 inserted channel config */
    adc_inserted_channel_config(ADC2, 0, I_OUT_channel,     ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC2, 1, AC_OUT_channel,    ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC2, 2, AC_IN_channel,     ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC2, 3, REF_2_5_channel,   ADC_SAMPLETIME_55POINT5);

    /* ADC1 regular channel config */
    adc_regular_channel_config(ADC0, 0, I_PM_channel,       ADC_SAMPLETIME_1POINT5);    
    adc_regular_channel_config(ADC0, 1, U_BAT_channel,      ADC_SAMPLETIME_1POINT5);
    adc_regular_channel_config(ADC0, 2, TEMP_channel,       ADC_SAMPLETIME_1POINT5);
    adc_regular_channel_config(ADC0, 3, T_BAT_channel,      ADC_SAMPLETIME_1POINT5);
    





    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC2, ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC2, ADC_INT_FLAG_EOIC);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC2, ADC_INT_EOIC);

    /* enable ADC0 interface */
    adc_enable(ADC0);
    Delay_Ms(1);
    /* ADC0 calibration and reset calibration */
    adc_calibration_enable(ADC0);
    printf("%s() ADC2 calibration completed\n", __func__);

    /* enable ADC1 interface */
    adc_enable(ADC2);
    Delay_Ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);
    printf("%s() ADC0 calibration completed\n", __func__);

    adc_dma_mode_enable(ADC0);
    
    //start ADC2
    //adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);

    //start ADC0
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
    // printf("ADC0 status:  WDE=%d, EOC=%d, EOIC=%d, STIC=%d, STRC=%d\n",
    //     adc_flag_get(ADC0, ADC_FLAG_WDE),
    //     adc_flag_get(ADC0, ADC_FLAG_EOC),
    //     adc_flag_get(ADC0, ADC_FLAG_EOIC),
    //     adc_flag_get(ADC0, ADC_FLAG_STIC),
    //     adc_flag_get(ADC0, ADC_FLAG_STRC)
    // );
    //while(!adc_flag_get(ADC0, ADC_FLAG_EOC))
    while(!dma_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF))
    {
        Delay_Ms(1);
    };
    
        //clear the end of conversion flag
        adc_flag_clear(ADC0, ADC_FLAG_EOC);
        dma_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);
    // return regular channel sample value
    printf("ADC0 U_BAT_channel = %.3f\n", hex_to_volt((uint16_t)adc_value[0]));
    printf("ADC0 TEMP_channel  = %.3f\n", hex_to_volt((uint16_t)adc_value[1]));
    printf("ADC0 T_BAT_channel = %.3f\n", hex_to_volt((uint16_t)adc_value[2]));
    printf("ADC0 I_PM_channel  = %.3f\n", hex_to_volt((uint16_t)adc_value[3]));



}

float hex_to_volt(int16_t val)
{
    return (float)(ADC_REF_VOLTAGE * val) / 4095;//volt;
}

uint16_t volt_to_hex(float val)
{
    return (uint16_t)(val * 4095 / ADC_REF_VOLTAGE);
    
    
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);
    
    /* initialize DMA data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 4;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);


}


uint16_t u_out_offset(void)
{
    uint32_t sum = 0;
    for(int i = 0; i < 2000; i++)
    {
        sum += adc_channel_sample(ADC2, AC_OUT_channel);

    }
    uint16_t res = (uint16_t)(sum / 2000);

    printf("%s() Uout_offset = %u\n", __func__, res);
    //printf("%s() Iout = %u\n", __func__, res);
    return res;
}

uint16_t current_offset(void)
{
    uint32_t sum = 0;
    for(int i = 0; i < 2000; i++)
    {
        sum += adc_channel_sample(ADC2, I_OUT_channel);

    }
    uint16_t res = (uint16_t)(sum / 2000);

    printf("%s() Iout_offset = %u\n", __func__, res);
    //printf("%s() Iout = %u\n", __func__, res);
    return res;
}

uint16_t u_input_offset(void)
{
    uint32_t sum = 0;
    for(int i = 0; i < 2000; i++)
    {
        sum += adc_channel_sample(ADC2, AC_IN_channel);

    }
    uint16_t res = (uint16_t)(sum / 2000);

    printf("%s() Uin_offset = %u\n", __func__, res);
    //printf("%s() Iout = %u\n", __func__, res);
    return res;
}

uint16_t ref_2_5(void)
{
    uint32_t sum = 0;
    for(int i = 0; i < 2000; i++)
    {
        sum += adc_channel_sample(ADC2, REF_2_5_channel);

    }
    uint16_t res = (uint16_t)(sum / 2000);

    printf("%s() REF 2.5V = %u\n", __func__, res);
    return res;
}

/*!
    \brief      ADC channel sample
    \param[in]  none
    \param[out] none
    \retval     none
*/

uint16_t adc_channel_sample(uint32_t adc_periph, uint8_t channel)
{
    // ADC regular channel config
    adc_regular_channel_config(adc_periph, 0U, channel, ADC_SAMPLETIME_7POINT5);
    // ADC software trigger enable
    adc_software_trigger_enable(adc_periph, ADC_REGULAR_CHANNEL);

    //wait the end of conversion flag
    while(!adc_flag_get(adc_periph, ADC_FLAG_EOC));
    //clear the end of conversion flag
    adc_flag_clear(adc_periph, ADC_FLAG_EOC);
    // return regular channel sample value
    return (adc_regular_data_read(adc_periph));

}


