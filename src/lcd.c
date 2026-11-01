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
#include "lcd_font.h"

#define I2C0_OWN_ADDRESS7       0x72
#define I2C0_SLAVE_ADDRESS7     0x3C


/* 1 - only one data byte (either command or RAM data) will follow.
0 - series of data bytes (either command or RAM data) may follow */
#define TIC32AN_CONTROL_CMD     0x80U

#define TIC32AN_CONTROL_DATA    0x40U

#define PCF8531_WIDTH           128
#define PCF8531_BANKS           4

#define TIC32AN_RAM_SIZE            640U
#define TIC32AN_VISIBLE_RAM_SIZE    512U


extern int16_t enc_get_position(void);


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


static const uint8_t tic32an_draw_PH[] = {
    TIC32AN_CONTROL_DATA,
    // 0x1F,
    // 0x05, 
    // 0x07,
    // 0x00,
    // 0x1F,
    // 0x04,
    // 0x1F
    0x00,
    0x01,
   // 0x01,
    0x03,
    0x07,
    0x0F,
    0x1F,
    0x3F,
    0x7F,
    0xFF
    
    
};

static const uint8_t tic32an_set_home_sequence[] = {
    TIC32AN_CONTROL_CMD, 0x01, /* main command page */
    TIC32AN_CONTROL_CMD, 0x41, /* Y address = 0 */
    TIC32AN_CONTROL_CMD, 0x80  /* X address = 0 */
};

void lcd_test(void)
{
    printf("%s()\n", __func__);
//    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_set_home_sequence, sizeof(tic32an_set_home_sequence));
  //  i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_draw_PH, sizeof(tic32an_draw_PH));
    int16_t enc_position = enc_get_position();
    char buf[100] = {0};
    sprintf(buf, "Hello, World! %d", enc_position);
  //  lcd_puts(3, 0, buf);

    lcd_puts16(0, 0, "U=12,5 A");   /* строка 0 (банки 0-1) */
    lcd_puts16(0, 2, "P=1.8 W");    /* строка 2 (банки 2-3) */
}

void lcd_init(void)
{
    static const uint8_t tic32an_init_sequence[] = {
        // TIC32AN_CONTROL_CMD, 0x01, /* Co + H[1:0] independent command; select function and RAM command page (H[1:0] = 00) */
        // TIC32AN_CONTROL_CMD, 0x22, /* Co + chip enable(PD=0), vertical addressing(PV=1) */
        // TIC32AN_CONTROL_CMD, 0x09, /* Co + function and RAM command page select display setting command page H[1:0] = 01 */
        // TIC32AN_CONTROL_CMD, 0x0C, /* Co + display setting command page; set normal mode(D = 1; IM = 0 and E = 0) */
        // TIC32AN_CONTROL_CMD, 0x05, /* Co + select multiplex rate 1:34 */
        // TIC32AN_CONTROL_CMD, 0x01, /* Co + H[2:0] independent command; select function and RAM command page H[1:0] = 00 */
        // TIC32AN_CONTROL_CMD, 0x0A, /* Co + function and RAM command page; select HV-gen command page H[1:0] = 10 */
        // TIC32AN_CONTROL_CMD, 0x0B, /* Co + HV-gen command page; select voltage multiplication factor 5 S[1:0] = 11 */
        // TIC32AN_CONTROL_CMD, 0x22, /* Co + HV-gen command page; select temperature coefficient 2 TC[2:0] = 010 */
        // TIC32AN_CONTROL_CMD, 0x07, /* Co + HV-gen command page; select high VLCD programming range (PRS = 1); voltage multiplier off (HVE = 1) */
        // TIC32AN_CONTROL_CMD, 0xA0  /* Co + HV-gen command page; set VLCD = 7.71 V; VOP[6:0] = 0100000 */

        


        TIC32AN_CONTROL_CMD, 0x01, /* Co + H[1:0] independent command; select function and RAM command page (H[1:0] = 00) */
        TIC32AN_CONTROL_CMD, 0x20, /* Co + chip enable(PD=0), horisontal addressing(PV=0) */
        TIC32AN_CONTROL_CMD, 0x09, /* Co + function and RAM command page select display setting command page H[1:0] = 01 */
        TIC32AN_CONTROL_CMD, 0x0C, /* Co + display setting command page; set normal mode(D = 1; IM = 0 and E = 0) */
        TIC32AN_CONTROL_CMD, 0x05, /* Co + select multiplex rate 1:34 */
        TIC32AN_CONTROL_CMD, 0x14, /*-- Co + bias system: 1/9 */
        TIC32AN_CONTROL_CMD, 0x01, /* Co + H[2:0] independent command; select function and RAM command page H[1:0] = 00 */
        TIC32AN_CONTROL_CMD, 0x0A, /* Co + function and RAM command page; select HV-gen command page H[1:0] = 10 */
        TIC32AN_CONTROL_CMD, 0x0A, /* Co + HV-gen command page; select voltage multiplication factor 5 S[1:0] = 2x */
        TIC32AN_CONTROL_CMD, 0x20, /* Co + HV-gen command page; select temperature coefficient 2 TC[2:0] = 010 */
        TIC32AN_CONTROL_CMD, 0x05, /* Co + HV-gen command page; select high VLCD programming range (PRS = 1); voltage multiplier off (HVE = 1) */ //Vlcd - LOW; Voltage multiplier ON
        TIC32AN_CONTROL_CMD, 0xDF /*-- Co + voltage multiplier on, low VLCD */ // Vlcd set

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

    //lcd reset pin
    gpio_init(RESET_lcd_gpio, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET_lcd_pin);
    Delay_Ms(10);
    
    gpio_bit_reset(RESET_lcd_gpio, RESET_lcd_pin);
    Delay_Ms(100);
    gpio_bit_set(RESET_lcd_gpio, RESET_lcd_pin);
    Delay_Ms(10);

    i2c_deinit(I2C0);
    /* cofigure I2C clock */
    i2c_clock_config(I2C0, 50000, I2C_DTCY_2);//I2C_DTCY_16_9
    /* cofigure I2C address */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);


    /* display driver init */
    i2c_transmit(I2C0_SLAVE_ADDRESS7, tic32an_init_sequence, sizeof(tic32an_init_sequence));

    /* clear RAM */
    i2c_begin_write(I2C0_SLAVE_ADDRESS7<<1);
    i2c_write_byte(TIC32AN_CONTROL_DATA);
    for(uint16_t i = 0; i < TIC32AN_RAM_SIZE; ++i) {
        i2c_write_byte(0x00);
    }
    i2c_end_write();

}

void i2c_transmit(uint8_t address, const uint8_t *data, uint16_t length)
{
    if((NULL == data) || (0U == length)) {
        return;
    }

    i2c_begin_write(address << 1);
    if(debug_i2c) {
        printf("%02X ", address << 1);
    }
    for(uint16_t i = 0; i < length; ++i) {
        i2c_write_byte(data[i]);
        if(debug_i2c) {
            printf("%02X ", data[i]);
        }
    }
    i2c_end_write();
     if(debug_i2c) printf("\n");
}

void lcd_en_backlight(void)
{
    gpio_bit_set(LCD_backlight_gpio, LCD_backlight_pin);
}

void lcd_dis_backlight(void)
{
    gpio_bit_reset(LCD_backlight_gpio, LCD_backlight_pin);
}