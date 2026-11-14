#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <stdint.h>
#include "gd32f30x.h"

/* Encoder PEC12R-4115F-S0012 pins */
#define ENC_gpio        GPIOA
#define ENCA_pin        GPIO_PIN_6
#define ENCB_pin        GPIO_PIN_7
#define ENCSW_pin       GPIO_PIN_5

#define ENC_TIMER       TIMER2
#define ENC_TIMER_RCU   RCU_TIMER2


void enc_init(void);
int16_t enc_get_position(void);

#endif
