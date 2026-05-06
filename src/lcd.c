#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_i2c.h"
//#include "gd32f30x_timer.h"
#include "gd32f30x_adc.h"

#include <stdio.h>
#include "gd32f30x_eval.h"
#include <stdint.h>


#include "systick.h"
#include "global.h"
#include "lcd.h"

#define I2C0_OWN_ADDRESS7      0x72
#define I2C0_SLAVE_ADDRESS7    0x82
#define TIC32AN_CONTROL_CMD    0x80U
#define TIC32AN_CONTROL_DATA   0x40U
#define TIC32AN_RAM_SIZE       640U
#define TIC32AN_VISIBLE_RAM_SIZE 512U

static void i2c_wait_for_bus_idle(void)
{
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
}

static void i2c_begin_write(uint8_t address)
{
    i2c_wait_for_bus_idle();
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

    i2c_master_addressing(I2C0, address, I2C_TRANSMITTER);
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
}

static void i2c_write_byte(uint8_t data)
{
    i2c_data_transmit(I2C0, data);
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
}

static void i2c_end_write(void)
{
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0) & I2C_CTL0_STOP);
}


void lcd_init(void)
{
    static const uint8_t tic32an_init_sequence[] = {
        TIC32AN_CONTROL_CMD, 0x01, /* main command page */
        TIC32AN_CONTROL_CMD, 0x20, /* chip enable, horizontal addressing */
        TIC32AN_CONTROL_CMD, 0x09, /* extended page select */
        TIC32AN_CONTROL_CMD, 0x0C, /* normal display mode */
        TIC32AN_CONTROL_CMD, 0x05, /* MUX rate: 1/34 */
        TIC32AN_CONTROL_CMD, 0x14, /* bias system: 1/9 */
        TIC32AN_CONTROL_CMD, 0x01, /* return to main command page */
        TIC32AN_CONTROL_CMD, 0x0A, /* extended page select */
        TIC32AN_CONTROL_CMD, 0x0A, /* voltage multiplier: 2x */
        TIC32AN_CONTROL_CMD, 0x20, /* temperature coefficient: 0 */
        TIC32AN_CONTROL_CMD, 0x05, /* voltage multiplier on, low VLCD */
        TIC32AN_CONTROL_CMD, 0xC4  /* VLCD set */
    };
    static const uint8_t tic32an_set_home_sequence[] = {
        TIC32AN_CONTROL_CMD, 0x01, /* main command page */
        TIC32AN_CONTROL_CMD, 0x40, /* Y address = 0 */
        TIC32AN_CONTROL_CMD, 0x80  /* X address = 0 */
    };

    printf("%s()\n", __func__);
    
    //i2c pins
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_I2C0);
    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(I2C_gpio, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_pin | I2C_SDA_pin);
    
    //backlight pin
    gpio_init(LCD_backlight_gpio, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_backlight_pin);


    i2c_deinit(I2C0);
    /* cofigure I2C clock */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* cofigure I2C address */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);


    /* display driver init */
    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_init_sequence, sizeof(tic32an_init_sequence));
    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_set_home_sequence, sizeof(tic32an_set_home_sequence));

    i2c_begin_write(I2C0_SLAVE_ADDRESS7);
    i2c_write_byte(TIC32AN_CONTROL_DATA);
    for(uint16_t i = 0; i < TIC32AN_RAM_SIZE; ++i) {
        i2c_write_byte(0x00);
    }
    i2c_end_write();
}

void lcd_test(void)
{
    static const uint8_t tic32an_set_home_sequence[] = {
        TIC32AN_CONTROL_CMD, 0x01, /* main command page */
        TIC32AN_CONTROL_CMD, 0x40, /* Y address = 0 */
        TIC32AN_CONTROL_CMD, 0x80  /* X address = 0 */
    };
    static const uint8_t square_4x4[] = {
        0x0FU, 0x0FU, 0x0FU, 0x0FU
    };

    printf("%s()\n", __func__);

    gpio_bit_set(LCD_backlight_gpio, LCD_backlight_pin);

    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_set_home_sequence, sizeof(tic32an_set_home_sequence));
    i2c_begin_write(I2C0_SLAVE_ADDRESS7);
    i2c_write_byte(TIC32AN_CONTROL_DATA);
    for(uint16_t i = 0; i < TIC32AN_VISIBLE_RAM_SIZE; ++i) {
        i2c_write_byte(0x00);
    }
    i2c_end_write();

    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_set_home_sequence, sizeof(tic32an_set_home_sequence));
    i2c_begin_write(I2C0_SLAVE_ADDRESS7);
    i2c_write_byte(TIC32AN_CONTROL_DATA);
    for(uint16_t i = 0; i < sizeof(square_4x4); ++i) {
        i2c_write_byte(square_4x4[i]);
    }
    i2c_end_write();
}


void i2c_transmit(uint8_t address, const uint8_t *data, uint16_t length)
{
    if((NULL == data) || (0U == length)) {
        return;
    }

    i2c_begin_write(address);
    for(uint16_t i = 0; i < length; ++i) {
        i2c_write_byte(data[i]);
    }
    i2c_end_write();
}
