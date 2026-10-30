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

extern void periodic_read_mei_buf(int64_t itime);

extern uint16_t voltageRMSBuffer[SIN_TABLE_SIZE];

uint16_t mainsStartupCount = 0;


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

            //adc_config();

         //   Delay_Ms(100);
            //relay_config();

            

            //zero_detect_config();
            //period_timer_config();
            //config
           // tim1_gpio_config();
            //tim1_config();

           // tim0_gpio_config();
            //();
          //  Delay_Ms(10);
            //charge_relay_enable();

            //debug
            //inpit_relay_enable();
            //input_relay_disable();
            lcd_init();
            enc_init();
            sound_config();


            systemState = WAIT_MODE;
        }
        break;
        case WAIT_MODE:
            int16_t enc_position = enc_get_position();
            static int16_t prev_enc_position = 0;
            if(enc_position != prev_enc_position)
            {
                printf("Encoder position: %d\n", enc_position);
                prev_enc_position = enc_position;
            }
            cnt++;
            if(cnt > 1000) cnt = 0;
            if(!cnt)
            {
                
                int64_t tm = get_sys_time_ns();
                print_time_s_ns(tm);
                printf("%s() [%u] %u\n", __func__, systemState, cnt);
                gpio_output_bit_get(GPIOB, GPIO_PIN_15) ? lcd_dis_backlight() : lcd_en_backlight();
                lcd_test();
                panel_to_pb_transmit("Hello from Panel!\n", 20);

                //panel_receieve();
                periodic_read_mei_buf(itime);
                sound_(0);

            }
           // systemState = INVERTER_MODE;
            break;

            if (mainsState == MAINS_NOT_OK)
            {
                if (batteryState == BATTERY_UNDERVOLTAGE)
                {
                    systemState = SYSTEM_ERROR;
                    errorState = BATTERY_UNDERVOLTAGE_FAULT;
                }
                else if (batteryState == BATTERY_OVERVOLTAGE)
                {
                    systemState = SYSTEM_ERROR;
                    errorState = BATTERY_OVERVOLTAGE_FAULT;
                }                  
                else 
                {
                    if (dcLinkState == DC_LINK_UNDERVOLTAGE)
                    {
                        systemState = SYSTEM_ERROR;
                        errorState = DC_LINK_UNDERVOLTAGE_FAULT;
                    }
                    else if (dcLinkState == DC_LINK_OVERVOLTAGE)
                    {
                        systemState = SYSTEM_ERROR;
                        errorState = DC_LINK_OVERVOLTAGE_FAULT;
                    }
                    else
                    {
                        mainsStartupCount++;
                        if (mainsStartupCount >= 10000)
                        {
                            mainsStartupState = MAINS_NOT_OK;
                            systemState = SYSTEM_ERROR;
                            errorState = MAINS_STARTUP_FAULT;
                        }
                    }
                }
            }
            else if (mainsState == MAINS_OK)
            {
                if (dcLinkState == DC_LINK_UNDERVOLTAGE)
                {
                    systemState = SYSTEM_ERROR;
                    errorState = DC_LINK_UNDERVOLTAGE_FAULT;
                }
                else if (dcLinkState == DC_LINK_OVERVOLTAGE)
                {
                    systemState = SYSTEM_ERROR;
                    errorState = DC_LINK_OVERVOLTAGE_FAULT;
                }
                else
                {
                    if (batteryState == BATTERY_OVERVOLTAGE)
                    {
                        systemState = SYSTEM_ERROR;
                        errorState = BATTERY_OVERVOLTAGE_FAULT;
                    }
                    else
                    {
                        systemState = BATTERY_CHARGER_MODE;
                                            /* If AC mains is good and 
                                               wrong battery is not
                                               connected then switch to 
                                               Battery Charger mode */
                        batteryChargerTurnOnRoutine();
                    }
                }
            }
        break;
        case BATTERY_CHARGER_MODE:   //battery charger mode
        {
            // if (mainsState == MAINS_OK)
            // {
            //     batteryChargingRoutine();
            //     if (dcLinkState == DC_LINK_UNDERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = DC_LINK_UNDERVOLTAGE_FAULT;
            //     }
            //     else if (dcLinkState == DC_LINK_OVERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = DC_LINK_OVERVOLTAGE_FAULT;
            //     }
            // }
            // else if (mainsState == MAINS_NOT_OK)
            // {
            //     if (batteryState == BATTERY_UNDERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = BATTERY_UNDERVOLTAGE_FAULT;
            //     }
            //     else if (batteryState == BATTERY_OVERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = BATTERY_OVERVOLTAGE_FAULT;
            //     }
            //     else
            //     {
            //         if (dcLinkState == DC_LINK_UNDERVOLTAGE)
            //         {
            //             systemState = SYSTEM_ERROR;
            //             errorState = DC_LINK_UNDERVOLTAGE_FAULT;
            //         }
            //         else if (dcLinkState == DC_LINK_OVERVOLTAGE)
            //         {
            //             systemState = SYSTEM_ERROR;
            //             errorState = DC_LINK_OVERVOLTAGE_FAULT;
            //         }
            //         else
            //         {
            //             systemState = INVERTER_MODE;
            //             pushpullTurnOnRoutine();
            //             softStartRoutine();
            //             inverterTurnOnRoutine();
            //         }
            //     }
            // }
        }
        break;
        case INVERTER_MODE:   //inverter mode
            if (fRMSCalculation == READY_TO_CALCULATE)
		    {
                uint16_t inverterVoltageMax = VectorMax(RMS_BUFFER_SIZE, voltageRMSBuffer);
		        uint16_t inverterVoltageMin = VectorMin(RMS_BUFFER_SIZE, voltageRMSBuffer);
                if (inverterImbalanceCount >= 16)
		        {
        		    /* Adjustment for DC offset in output voltage */
                	if (inverterVoltageImbalance > 40)
                	{
                        inverterOffsetAdjustment -= 5;
                        if (inverterOffsetAdjustment <= -200)
                	    {
                		    inverterOffsetAdjustment = -200;
                	    }
                    }
                	else if (inverterVoltageImbalance < -40)
                	{
                        inverterOffsetAdjustment += 5;
                        if (inverterOffsetAdjustment >= 200)
                	    {
                		    inverterOffsetAdjustment = 200;
                	    }
                    }
                
    		        inverterImbalanceCount = 0;
    		        inverterVoltageImbalance = 0;    		    
    		    }
    		    else
    		    {
        		    inverterImbalanceCount++;
        		    inverterVoltageImbalance += inverterVoltageMax + inverterVoltageMin;    
    		    }

		        fRMSCalculation = CALCULATION_DONE;
            }


            // if (mainsState == MAINS_OK)
            // {
            //     if (dcLinkState == DC_LINK_UNDERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = DC_LINK_UNDERVOLTAGE_FAULT;
            //     }
            //     else if (dcLinkState == DC_LINK_OVERVOLTAGE)
            //     {
            //         systemState = SYSTEM_ERROR;
            //         errorState = DC_LINK_OVERVOLTAGE_FAULT;
            //     }
            //     else
            //     {
            //         if (batteryState == BATTERY_OVERVOLTAGE)
            //         {
            //             systemState = SYSTEM_ERROR;
            //             errorState = BATTERY_OVERVOLTAGE_FAULT;
            //         }
            //         else
            //         {
            //             systemState = BATTERY_CHARGER_MODE;
            //                                 /* If AC mains is good and 
            //                                    wrong battery is not
            //                                    connected then switch to 
            //                                    Battery Charger mode */
            //             batteryChargerTurnOnRoutine();
            //         }
            //     }
            // }
            // else if (mainsState == MAINS_NOT_OK)
            // {
			//     if (batteryState == BATTERY_UNDERVOLTAGE)
			//     {
			//         systemState = SYSTEM_ERROR;
			//         errorState = BATTERY_UNDERVOLTAGE_FAULT;
			//     }
			//     else if (batteryState == BATTERY_OVERVOLTAGE)
			//     {
			//         systemState = SYSTEM_ERROR;
			//         errorState = BATTERY_OVERVOLTAGE_FAULT;
			//     }
			//     else
			//     {
            //         if (dcLinkState == DC_LINK_UNDERVOLTAGE)
            //         {
            //             systemState = SYSTEM_ERROR;
            //             errorState = DC_LINK_UNDERVOLTAGE_FAULT;
            //         }
			// 	    else if (dcLinkState == DC_LINK_OVERVOLTAGE)
			//         {
			//             systemState = SYSTEM_ERROR;
			//             errorState = DC_LINK_OVERVOLTAGE_FAULT;
			//         }
			// 	}
		    // }



        break;
        case SYSTEM_ERROR:      //system error
        {
            /*1. Turn OFF PWM outputs to disable inverter and push-pull
               boost converter */

            
            /*2. Disable state machine because system is in error */

            
            /*3. Turn OFF Battery Charger when system is in Error */

            
            /*4. Connect Mains to UPS output when system is in Error */

            
            if (dcLinkState == DC_LINK_UNDERVOLTAGE)
            {
                /* DC_LINK_UNDERVOLTAGE is a hard fault */
                errorState = DC_LINK_UNDERVOLTAGE_FAULT;
                printf("DC Link Undervoltage Fault\n");
            }
            
            if (dcLinkState == DC_LINK_OVERVOLTAGE)
            {
                /* DC_LINK_OVERVOLTAGE is a hard fault */
                errorState = DC_LINK_OVERVOLTAGE_FAULT;
                printf("DC Link Overvoltage Fault\n");
            }
            
            if (batteryState == BATTERY_UNDERVOLTAGE)
            {
                errorState = BATTERY_UNDERVOLTAGE_FAULT;
                printf("Battery Undervoltage Fault\n");
            }
            
            if (batteryState == BATTERY_OVERVOLTAGE)
            {
                errorState = BATTERY_OVERVOLTAGE_FAULT;
                printf("Battery Overvoltage Fault\n");
            }
            if (inverterCurrentState == INVERTER_OVERCURRENT)
            {
                errorState = INVERTER_OVERCURRENT_FAULT;
                printf("Inverter Overcurrent Fault\n");
            }
            
            if (inverterDriverState == DRIVER_FAULT_ACTIVE)
            {
                errorState = INVERTER_DRIVER_FAULT;
                printf("Driver Fault\n");
            }

            if (mainsStartupState == MAINS_NOT_OK)
            {
                errorState = MAINS_STARTUP_FAULT;
                printf("Mains Startup Fault\n");
            }
        }
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
    //set pwm to nominal value
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, UP_TO_DOWN_THR);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);

    

    // /* Find out starting sample for inverter based on output capacitor voltage */
    // if (mainsQuadrant == FIRST_QUADRANT)
    // {
    //     sampleCount = mainsSampleSearch(mainsVoltage);
    // }
    // else if (mainsQuadrant == SECOND_QUADRANT)
    // {
    //     sampleCount = mainsSampleSearch(mainsVoltage) + 128;
    // }
    // else if (mainsQuadrant == THIRD_QUADRANT)
    // {
    //     sampleCount = mainsSampleSearch(mainsVoltage) + 256;
    // }
    // else if (mainsQuadrant == FOURTH_QUADRANT)
    // {
    //     sampleCount = mainsSampleSearch(mainsVoltage) + 384;
    // }
 
    // mainsQuadrant = UNKNOWN_QUADRANT;

    // temp4 = 0;								/* Purge control loop history */
    // prevFilteredVo = 0;						/* Purge control loop history */


    /* Turn ON PWM outputs to enable inverter output */
    timer_primary_output_config(TIMER0, ENABLE);
    
    Delay_Ms(1);
}

void batteryChargerTurnOnRoutine(void)
{
    // fSwitchRelay = NOT_READY_TO_SWITCH;
    // while(fSwitchRelay == NOT_READY_TO_SWITCH); 
    //                                         /* Before turning ON battery charger
    //                                            mode, wait till inverter output 
    //                                            reaches zero crossing. Switching
    //                                            of output relay near the zero
    //                                            crossing will reduce stress on 
    //                                            both relay and Inverter IGBTs */

    // LATCbits.LATC0 = 0;                     /* Connect mains to UPS output
    //                                            when in Battery Charger mode */
    Delay_Ms(10);                           /* Wait 10ms to allow relay to switch
    										   During this time, the inverter is 
    										   still operating and will be turned
    										   OFF near the zero crossing to 
    										   reduce stress on the circuitry. */
                                               
    
    /* Turn OFF PWM outputs to disable inverter */
   timer_primary_output_config(TIMER0, DISABLE);
    
    // /* Disable Leading edge blanking for PWM3 */
    // LEBCON3 = 0x07F8;                       /* Disable LEB as PWM outputs are 
    //                                            in override.
    //                                            Due to errata on A0 and A1
    //                                            silicon word-writes must be
    //                                            used for LEBCONx */

    // LATCbits.LATC10 = 1;                    /* Bypass inverter series
    //                                            resistor because DC Link
    //                                            capacitors are charged */

    // Delay_Ms(12);                           /* Wait for relays to switch before
    //                                            turning ON battery charger */
                                               
    // SDC4 = MIN_CHARGING_CURRENT;            /* Set minimum charging current */
                                               
    // IOCON4bits.OVRENL = 0;                  /* Turn ON PWM4L output for battery
    //                                            charger control */

    // LATCbits.LATC7 = 1;                     /* Enable Battery Charger control */

}

void pushpullTurnOnRoutine(void)
{

                                   
}