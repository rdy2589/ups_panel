#include "gd32f30x.h"
#include "gd32f30x_eval.h"
#include "systick.h"
#include "app_time.h"
#include <stdio.h>

#include "global.h"
#include "sin_timer.h"
#include "sin_table.h"
#include "driver_pwm.h"
#include "debug.h"
#include "relay.h"
#include "adc.h"
#include "lcd.h"

#include "state_machine.h"
#include "encoder.h"
#include "usart.h"
#include "input_data.h"
#include "ui.h"

extern int periodic_read_mei_buf(int64_t itime);

extern uint16_t voltageRMSBuffer[SIN_TABLE_SIZE];
extern struct item_t settings_values[];

uint16_t mainsStartupCount = 0;
struct pb_msg_t pbReceieved;

/* Main state task function */
void main_state_task(int64_t itime)
{
    static uint16_t cnt = 0;
    switch(systemState)
    {
        case SYSTEM_STARTUP:
        {
            print_time_s_ns(itime);
            printf("%s() [%u]\n", __func__, systemState);

            //generate_sin_table();

            nvic_config();
            __enable_irq();  // Команда CMSIS


            lcd_init();
            enc_init();
            sound_config();
            ui_init();
            relay_config();
            led_config();
            btn_led_config();
            ext_input_config();

            inverterState.mode = INV_MODE_UNKNOWN;
            inverterState.battery = BATT_UNKNOWN;
            inverterState.mlf = MLF_UNKNOWN;
            inverterState.mlf_bat  = MLF_UNKNOWN;
            inverterState.mlf_ovh  = MLF_UNKNOWN;

            systemState = WAIT_MODE;
        }
        break;
        case WAIT_MODE:
            // encoder position reading
            int16_t enc_position = enc_get_position();
            int16_t enc_diff = 0;
            static int16_t prev_enc_position = 0;
            if(enc_position != prev_enc_position)
            {
                static int16_t enc_acc = 0;
                printf("Encoder position: %d\n", enc_position);
                enc_acc += (enc_position - prev_enc_position);
                if(enc_acc >= 4 || enc_acc <= -4)
                {
                    enc_diff = enc_acc / 4;
                    enc_acc = 0;
                }
                prev_enc_position = enc_position;
            }

            // 2. Опрос физических кнопок (активный уровень - 0, так как IPU)
            static uint8_t btn1_prev = 1, btn2_prev = 1, btn_sw_prev = 1;
            uint8_t btn1_pressed = 0, btn2_pressed = 0, btn_sw_pressed = 0;

            uint8_t btn1_state = gpio_input_bit_get(BT_MENU_gpio, BT_MENU_pin);
            uint8_t btn2_state = gpio_input_bit_get(BT_OFF_gpio, BT_OFF_pin);
            uint8_t btn_sw_state = gpio_input_bit_get(ENC_gpio, ENCSW_pin);

            // Логика выделения переднего фронта нажатия (клик)
            if (btn1_state == 0 && btn1_prev == 1) { btn1_pressed = 1; sound_(settings_values[5].value, 100); }
            if (btn2_state == 0 && btn2_prev == 1) { btn2_pressed = 1; sound_(settings_values[5].value, 100); }
            if (btn_sw_state == 0 && btn_sw_prev == 1) btn_sw_pressed = 1;

            btn1_prev = btn1_state;
            btn2_prev = btn2_state;
            btn_sw_prev = btn_sw_state;

            // 3. Вызов обработчика пользовательского интерфейса
            ui_update(enc_diff, btn1_pressed, btn2_pressed, btn_sw_pressed);


            if(settings_values[4].value == 1) 
            {
                btn_led_enable();
                lcd_en_backlight();
            }
            else
            {
                lcd_dis_backlight();
                btn_led_disable();
            }

            cnt++;
            if(cnt > 10000) cnt = 0;
            if(!cnt)
            {
                int64_t tm = get_sys_time_ns();
                print_time_s_ns(tm);
                printf("%s() [%u] %u\n", __func__, systemState, cnt);
                //lcd_test();
                //panel_to_pb_transmit("Hello from Panel!\n", 20);

                //panel_receieve();

                uint8_t buf[20];
                int len = 0;
                //little endian style
                int i = 0;
                buf[i++] = 0x22; // state low byte
                buf[i++] = 0x11; // state high byte
                buf[i++] = 2201 & 0xFF; // uMains low byte
                buf[i++] = 2201 >> 8; // uMains high byte
                buf[i++] = 2302 & 0xFF; // uInvertor low byte
                buf[i++] = 2302 >> 8; // uInvertor high byte
                buf[i++] = 2403 & 0xFF; // iInvertor low byte
                buf[i++] = 2403 >> 8; // iInvertor high byte
                buf[i++] = 2504 & 0xFF; // uBattery low byte 
                buf[i++] = 2504 >> 8; // uBattery high byte
                buf[i++] = 2605 & 0xFF; // iBattery low byte
                buf[i++] = 2605 >> 8; // iBattery high byte
        
                send_piv_msg(itime, 0x55, buf, i);
            }

            if (periodic_read_mei_buf(itime) == OK)
            {
                //if message unpacked to struct pb_msg_t
                //make decision about system state and mode
                if(pbReceieved.updated)
                {
                    unpackState(pbReceieved.state, &inverterState);
                    pbReceieved.updated = 0;
                }
            }
//pbReceieved.state = MLF_MASK | BAT_MLF_MASK | OVH_MLF_MASK | INV_MODE_MASK;
//pbReceieved.updated = 1;
//unpackState(pbReceieved.state, &inverterState);

//inverterState.mode = INV_MODE_BATTERY;
inverterState.mlf_bat = MLF_NOT_OK;
//inverterState.mlf_ovh = MLF_NOT_OK;

            sound_proceed();
            relayProceed();
            led_proceed();
            ext_input_proceed();
            
        if(!gpio_output_bit_get(GPIOD, GPIO_PIN_8))
            gpio_bit_set(GPIOD, GPIO_PIN_8);
        else
            gpio_bit_reset(GPIOD, GPIO_PIN_8); 
            
        break;
        case BATTERY_CHARGER_MODE:   //battery charger mode
        {

        }
        break;
        case INVERTER_MODE:   //inverter mode

        break;
        case SYSTEM_ERROR:      //system error
        

        break;
        case DEBUG_MODE:

        break;
        default:

    }


}

void ctrl_pins_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOD);
    
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    printf("%s()\n", __func__);
}

//ON_AUX_MCU
void aux_enable(void)
{
    gpio_bit_set(GPIOD, GPIO_PIN_11);
    //printf("%s()\n", __func__);
}

//ON_AUX_MCU
void aux_disable(void)
{
    gpio_bit_reset(GPIOD, GPIO_PIN_11);
    //printf("%s()\n", __func__);
}




uint16_t VectorMax(uint16_t length, const uint16_t *buffer)
{
    uint16_t max = buffer[0];

    for (uint16_t i = 1; i < length; i++)
    {
        if (buffer[i] > max)
        {
            max = buffer[i];
        }
    }

    return max;
}

uint16_t VectorMin(uint16_t length, const uint16_t *buffer)
{
    uint16_t min = buffer[0];

    for (uint16_t i = 1; i < length; i++)
    {
        if (buffer[i] < min)
        {
            min = buffer[i];
        }
    }

    return min;
}


void inverterTurnOnRoutine(void)
{

}

void batteryChargerTurnOnRoutine(void)
{
 


}

void unpackState(uint16_t state, struct inverter_state_t *inverterState)
{
    inverterState->mlf = (state & MLF_MASK) ? MLF_NOT_OK : MLF_OK;
    inverterState->mlf_bat = (state & BAT_MLF_MASK) ? MLF_NOT_OK : MLF_OK;
    inverterState->mlf_ovh = (state & OVH_MLF_MASK) ? MLF_NOT_OK : MLF_OK;
    inverterState->mode = (state & INV_MODE_MASK) ? INV_MODE_BATTERY : INV_MODE_LINE;
}

void relayProceed(void)
{
    if(inverterState.mlf == MLF_NOT_OK)
    {
        mlf_relay_disable();
    }
    else
    {
        mlf_relay_enable();
    }

    if(inverterState.mlf_bat == MLF_NOT_OK)
    {
        battery_relay_disable();
    }
    else
    {
        battery_relay_enable();
    }

    if(inverterState.mlf_ovh == MLF_NOT_OK)
    {
        overheat_relay_disable();
    }
    else
    {
        overheat_relay_enable();
    }
}

void led_proceed(void)
{
    if(inverterState.mlf_bat == MLF_NOT_OK)
    {
        led_bat_mlf_enable();
    }
    else
    {
        led_bat_mlf_disable();
    }

    if(inverterState.mode == INV_MODE_BATTERY)
    {
        led_bat_enable();
        led_line_disable();
    }
    else
    {
        led_bat_disable();
        led_line_enable();
    }

    if(inverterState.mlf_ovh == MLF_NOT_OK)
    {
        led_ovh_enable();
    }
    else
    {
        led_ovh_disable();
    }
}

void ext_input_proceed(void)
{
    if(!get_ext_start()) 
    {
        //ext start enabled
        inverterState.mode = INV_MODE_BATTERY;
    }
    else {
        inverterState.mode = INV_MODE_LINE;
    }
    if(!get_ext_block())
    {
        //ext block enabled
    }
}