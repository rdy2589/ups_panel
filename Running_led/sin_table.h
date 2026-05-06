#ifndef __SIN_TABLE_H__
#define __SIN_TABLE_H__



#define VOLTAGE_OPEN_LOOP       0
#define CURRENT_OPEN_LOOP       0
#define VOLTAGE_OAMP_GAIN       149.09f
#define VOLTAGE_DIVIDER         1.512f
#define CURRENT_SENSITIVITY     0.066f  // 66 mV/A для ACS733
#define ADC_REF_VOLTAGE         3.286f
#define OUTPUT_VOLTAGE_SETTING  220    /* 220V */
#define OUTPUT_VOLTAGE_AMPL     ((float)(OUTPUT_VOLTAGE_SETTING * 1.4f))
#define CURRENT_MAX             20.0f   // Максимальный ток
#define SIN_TABLE_SIZE          200
#define PI                      3.14159265358979323846f
#define PWM_PERIOD              ((uint16_t)6000)
#define PWM_MAX                 ((uint16_t)(PWM_PERIOD / 2))
#define PWM_MID                 ((uint32_t)(PWM_PERIOD / 2))
#define UP_TO_DOWN_THR          ((uint32_t)(PWM_MID / 2))
#define PERIOD_POSITIVE_ADJ     ((int16_t)(240))      //+ 4Hz
#define PERIOD_NEGATIVE_ADJ     ((int16_t)(-240))      //- 4Hz

#define MAINS_PERIOD            (uint16_t)(20000)   //20ms
#define MAINS_PERIOD_THR        (uint16_t)(4000)    //2ms
#define HYST_DELAY              (uint16_t)(1000)     //delay between mains ZC and comparator output (us)

void generate_sin_table(void);

#endif
