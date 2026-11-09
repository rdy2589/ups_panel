#ifndef __RELAY_H__
#define __RELAY_H__

#define RELAY_gpio          GPIOC
#define RELAY_OVH_pin       GPIO_PIN_0
#define RELAY_BAT_pin       GPIO_PIN_1
#define RELAY_MLF_pin       GPIO_PIN_2

#define LED_gpio            GPIOE
#define LED_OVH_pin         GPIO_PIN_10
#define LED_BAT_pin         GPIO_PIN_8
#define LED_LINE_pin        GPIO_PIN_9
#define LED_BAT_MLF_pin     GPIO_PIN_11
#define LED_SW_ON_pin       GPIO_PIN_12
#define LED_SW_ENTER_pin    GPIO_PIN_13
#define LED_SW_ESC_pin      GPIO_PIN_14

#define EXT_INPUT_gpio      GPIOB
#define EXT_START_pin       GPIO_PIN_1
#define EXT_BLOCK_pin       GPIO_PIN_2

void overheat_relay_enable(void);
void overheat_relay_disable(void);
void battery_relay_enable(void);
void battery_relay_disable(void);
void mlf_relay_enable(void);
void mlf_relay_disable(void);
void led_ovh_enable(void);
void led_ovh_disable(void);
void led_bat_enable(void);
void led_bat_disable(void);
void led_line_enable(void);
void led_line_disable(void);
void led_bat_mlf_enable(void);
void led_bat_mlf_disable(void);
void btn_led_enable(void);
void btn_led_disable(void);
uint8_t get_ext_start(void);
uint8_t get_ext_block(void);


void relay_config(void);
void ext_input_config(void);
void led_config(void);
void btn_led_config(void);
void sound_config(void);
void sound_(uint16_t enabled, uint16_t time_ms);
void sound_proceed(void);

#endif
