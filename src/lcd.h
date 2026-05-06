#ifndef __LCD_H__
#define __LCD_H__

//TIC32AN i2c pins
#define I2C_gpio        GPIOB               //ADC012_IN1
#define I2C_SCL_pin     GPIO_PIN_6
#define I2C_SDA_pin     GPIO_PIN_7

#define LCD_backlight_gpio    GPIOB               //ADC012_IN1
#define LCD_backlight_pin     GPIO_PIN_15



void lcd_init(void);
void i2c_transmit(uint8_t address, const uint8_t *data, uint16_t length);
void lcd_test(void);
// void adc_config(void);
// uint16_t u_out_offset(void);
// uint16_t current_offset(void);
// uint16_t u_input_offset(void);
// uint16_t ref_2_5(void);
// void adc_dma_config(void);
// float hex_to_volt(int16_t val);
// void zero_detect_config(void);
// void period_timer_config();
// void period_timer_stop();
// void period_timer_start();
// uint16_t period_timer_get();


#endif
