#ifndef __ADC_H__
#define __ADC_H__

//ADC2 injected channels, DMA
#define I_OUT_gpio      GPIOA               //ADC012_IN1
#define I_OUT_pin       GPIO_PIN_1
#define I_OUT_channel   ADC_CHANNEL_1

#define REF_2_5_gpio    GPIOA               //ADC012_IN2
#define REF_2_5_pin     GPIO_PIN_2
#define REF_2_5_channel ADC_CHANNEL_2

#define AC_IN_gpio      GPIOC               //ADC012_IN10
#define AC_IN_pin       GPIO_PIN_0   
#define AC_IN_channel   ADC_CHANNEL_10

#define AC_OUT_gpio     GPIOC               //ADC012_IN11
#define AC_OUT_pin      GPIO_PIN_1
#define AC_OUT_channel  ADC_CHANNEL_11



//ADC0 regular channels, DMA
#define I_PM_gpio       GPIOA           //ADC012_IN3
#define I_PM_pin        GPIO_PIN_3
#define I_PM_channel    ADC_CHANNEL_3

#define U_BAT_gpio      GPIOA           //ADC01_IN7
#define U_BAT_pin       GPIO_PIN_7
#define U_BAT_channel   ADC_CHANNEL_7

#define TEMP_gpio       GPIOC           //ADC012_IN12
#define TEMP_pin        GPIO_PIN_2
#define TEMP_channel    ADC_CHANNEL_12

#define T_BAT_gpio      GPIOC           //ADC012_IN13
#define T_BAT_pin       GPIO_PIN_3
#define T_BAT_channel   ADC_CHANNEL_13

#define DU_gpio         GPIOC           //ADC01_IN14
#define DU_pin          GPIO_PIN_4
#define DU_channel      ADC_CHANNEL_14

#define DU1_gpio        GPIOC           //ADC01_IN15
#define DU1_pin         GPIO_PIN_5
#define DU1_channel     ADC_CHANNEL_15

//ADC?
#define BUS_gpio       GPIOB           //ADC01_IN8
#define BUS_pin        GPIO_PIN_0
#define BUS_channel    ADC_CHANNEL_8

#define I_BAT_gpio       GPIOB           //ADC01_IN9
#define I_BAT_pin        GPIO_PIN_1
#define I_BAT_channel    ADC_CHANNEL_9

//DAC
#define BUS_ADJ_gpio    GPIOA           //DAC_OUT0
#define BUS_ADJ_pin     GPIO_PIN_4   
//#define _channel   ADC_CHANNEL_

#define I_REF_gpio      GPIOA           //DAC_OUT1
#define I_REF_pin       GPIO_PIN_4   
//#define _channel   ADC_CHANNEL_



uint16_t adc_channel_sample(uint32_t adc_periph, uint8_t channel);
void adc_config(void);
uint16_t u_out_offset(void);
uint16_t current_offset(void);
uint16_t u_input_offset(void);
uint16_t ref_2_5(void);
void adc_dma_config(void);
float hex_to_volt(int16_t val);
void zero_detect_config(void);
void period_timer_config();
void period_timer_stop();
void period_timer_start();
uint16_t period_timer_get();


#endif
