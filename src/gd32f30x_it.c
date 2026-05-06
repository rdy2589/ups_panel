#include "gd32f30x_it.h"
#include "systick.h"
#include "gd32f30x_gpio.h"

#include "gd32f30x_eval.h"

#include "sin_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "debug.h"

extern float sin_table[SIN_TABLE_SIZE];
extern uint16_t voltageRMSBuffer[SIN_TABLE_SIZE];

extern void period_timer_stop();
extern void period_timer_start();
extern uint16_t period_timer_get();

static int32_t clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if(value < min_value) {
        return min_value;
    }
    if(value > max_value) {
        return max_value;
    }
    return value;
}

static uint16_t clamp_u16(uint16_t value, uint16_t min_value, uint16_t max_value)
{
    if(value < min_value) {
        return min_value;
    }
    if(value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t wrap_phase_error_i32(int32_t error_us, int32_t period_us)
{
    int32_t half_period_us = period_us / 2;

    while(error_us > half_period_us) {
        error_us -= period_us;
    }
    while(error_us < -half_period_us) {
        error_us += period_us;
    }

    return error_us;
}

enum
{
    MAINS_HALF_PERIOD_NOMINAL_US = (MAINS_PERIOD / 2U),
    MAINS_HALF_PERIOD_MIN_US = ((MAINS_PERIOD - MAINS_PERIOD_THR) / 2U),
    MAINS_HALF_PERIOD_MAX_US = ((MAINS_PERIOD + MAINS_PERIOD_THR) / 2U),
    PLL_HALF_PERIOD_FILTER_SHIFT = 3,
    PLL_PHASE_PROP_SHIFT = 4,
    PLL_PHASE_INT_SHIFT = 7,
    PLL_PHASE_TRIM_LIMIT = 120
};

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
extern volatile uint64_t sys_time;
void SysTick_Handler(void)
{
    //delay_decrement();
    sys_time++;
}


static uint16_t sin_index = 0;
static enum polarity_t polarity = 0;

void TIMER0_UP_IRQHandler(void)
{
    static uint8_t led_flag = 0;

    
    if(timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP) != RESET)
    {    
        /* clear TIMER interrupt flag */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);

        if(!gpio_output_bit_get(P36_GPIO_PORT, P36))
            gpio_bit_set(P36_GPIO_PORT, P36);
        else
            gpio_bit_reset(P36_GPIO_PORT, P36); 



        if(TIMER_CNT(TIMER0) > (TIMER_CAR(TIMER0)/2) )
        {
            //owerflow
            //gpio_bit_set(P30_GPIO_PORT, P30);

            sin_index = (sin_index + 1)  % SIN_TABLE_SIZE;
            if(sin_index == 0)
            {
                polarity = !polarity;
                ac_input_state.inv_phase = period_timer_get();
            }
        
        }
        else 
        {
            //underflow
            //gpio_bit_reset(P30_GPIO_PORT, P30);
            //run ADC measurement here

            //start I_out, AC_in, AC_out measure
            
            adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);


        }
    
    }

}

extern uint16_t inserted_data[4];
extern float hex_to_volt(int16_t val);

struct pid_t
{
    float inv_voltage_Kp;
    float inv_voltage_Ki;
    float inv_current_Kp;    
}; 


    
struct pid_t pid = {0.8f, 0.07f, 50.0f};    
float control_output = 0;
int inverterInputVoltage;
int inverterOutputVoltage;
int inverterOutputCurrent;
float CapCurrent=0, LoadCurrent = 0;
float inverterCurrentRef;
float inverterCurrentError;

float delVo_n =0.0f;
float FilteredVo_n =0.0f;
float prevFilteredVo_n =0.0f;
float InvCMALPHA = 0.7f;
float InvCMBETA = 0.3f;
float InvCbydt = 0.132f;	        //6.6uF*(dU/(1/20kHz)
float InvSafetyfactor = 0.99f;
float InvVMvoltagedecouple = 0.92f;
float InvRp = 0.2;//0.17/2.0;   //active resistance Rl+Rsw
float InvCMcurrentRa = 0.91;

float voltage_reference;
uint16_t ptr = 0;

float voltage_measured_n;           //normalised output voltage
float current_measured_n;           //normalised output current
float voltage_measured;

void ADC2_IRQHandler(void)
{
    static float volt_integrator = 0;
    static float control_output_prev = 0;

    static uint8_t polarity_prev = 0;

    uint8_t flag = 0;

    if(polarity_prev != polarity)
    {
        control_output_prev = 0;
        //volt_integrator = 0;
        //volt_integrator *= -0.6;
       // prevFilteredVo_n = 0;//prevFilteredVo_n;



    }
    polarity_prev = polarity;
    
    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC2, ADC_INT_FLAG_EOIC);
    /* read ADC inserted group data register */
    inserted_data[0] = adc_inserted_data_read(ADC2, ADC_INSERTED_CHANNEL_0);
    inserted_data[1] = adc_inserted_data_read(ADC2, ADC_INSERTED_CHANNEL_1);
    inserted_data[2] = adc_inserted_data_read(ADC2, ADC_INSERTED_CHANNEL_2);
    inserted_data[3] = adc_inserted_data_read(ADC2, ADC_INSERTED_CHANNEL_3);

    #if(!VOLTAGE_OPEN_LOOP)
        //sin_table all values[0...PI] > 0
        voltage_reference =  sin_table[sin_index]; //0...1
        if(!polarity) voltage_reference = -voltage_reference;
        // if((voltage_reference) < 0.1f) {
        //     // Уменьшаем интегральную составляющую вблизи нуля
        //     pid.inv_voltage_Ki = 0.03f;
        //     pid.inv_voltage_Kp = 1.5f;
        // } else {
        //     pid.inv_voltage_Ki = 0.07;
        //      pid.inv_voltage_Kp = 0.8f;
        // }
        
        inverterOutputVoltage = (int)inserted_data[1] - U_out_offset - inverterOffsetAdjustment;
        inverterOutputCurrent = (int)inserted_data[0] - I_out_offset;
        inverterInputVoltage = -((int)inserted_data[2] - U_input_offset);

        //float voltage_measured = fabs(hex_to_volt(inserted_data[1]) - U_zero) * 1.512 * 149.09;
        voltage_measured = hex_to_volt(inverterOutputVoltage) * VOLTAGE_DIVIDER * VOLTAGE_OAMP_GAIN;  //in Volts
        float current_measured = hex_to_volt(inverterOutputCurrent) / CURRENT_SENSITIVITY; //in Amps
        if(current_measured > CURRENT_MAX) volt_integrator = 0; //reset integrator if overcurrent, will help to recover after overload condition

        voltage_measured_n = voltage_measured / OUTPUT_VOLTAGE_AMPL;  //normalised output voltage
        current_measured_n = current_measured / CURRENT_MAX;          //normalised output current



        float volt_error = voltage_reference - voltage_measured_n;
        float prop_v = volt_error * pid.inv_voltage_Kp;
        volt_integrator += (volt_error * pid.inv_voltage_Ki);
        if(volt_integrator > 1.0f) volt_integrator = 1.0f;
        if(volt_integrator < -1.0f) volt_integrator = -1.0f;

        
            FilteredVo_n = InvCMALPHA * prevFilteredVo_n + InvCMBETA * voltage_measured_n;  //normalized filtered output voltage 
            delVo_n = FilteredVo_n - prevFilteredVo_n;
            CapCurrent = delVo_n * InvCbydt;
            LoadCurrent = (current_measured_n - CapCurrent) * InvSafetyfactor;

            inverterCurrentRef = prop_v + volt_integrator + LoadCurrent;

            inverterCurrentError = inverterCurrentRef - current_measured_n;
            
            //proportional + filter active resistance voltage compensation
            control_output = InvCMcurrentRa * inverterCurrentError + InvRp * current_measured_n;
            float temp3 = InvVMvoltagedecouple * voltage_measured_n;
            control_output = temp3 + control_output;

        #if(CURRENT_OPEN_LOOP)
            control_output = prop_v + volt_integrator;
        #endif
            
        if(control_output > 1.0f) control_output = 1.0f;    
        if(control_output < -1.0f) control_output = -1.0f;
        int16_t duty = (int16_t)(control_output * PWM_MAX);
 
    #else
        duty = sin_table[sin_index] * 3000;
    #endif

    if(duty > 0)
    {
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, (uint16_t)(abs(duty)));
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 0);
    } else
    {
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, (uint16_t)(abs(duty)));
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
    }




    ptr++;
    //if(!(ptr%10)) printf("%u\n", (uint16_t)inserted_data[1]);
    //if(!(ptr%10)) printf("%u\n", (uint16_t)volt_error);
    //if(!(ptr%10)) printf("%.2f\n", real_current);
    //if(!(ptr%10)) printf("%u\n", duty);
    //if(!(ptr%10)) printf("%.2f %.2f\n", reference, voltage_measured);
    //if(!(ptr%10)) printf("%.2f %.2f\n", reference, volt_error);
//    if(!(ptr%10)) printf("%.2f %u %u\n", reference, duty, polarity);

    //buf[ptr++] = (uint16_t)inserted_data[1];
    // if(ptr == 1000) ptr = 0;
    // if(ptr == 0) 
    // for(int i = 0; i < 1000; i++)
    //     printf("%.3f\n", hex_to_volt((uint16_t)buf[i]));
    
    /* Save previous filtered output voltage */
	prevFilteredVo_n = FilteredVo_n;


    // if (fRMSCalculation == CALCULATION_DONE)
    // {
    //     fRMSCalculation = READY_TO_COLLECT_DATA;
    // }
    // /* Save Measured Voltage */
    // //voltageRMSBuffer[RMSBufferCount] = (int)inserted_data[1];
    // if (fRMSCalculation == READY_TO_COLLECT_DATA)
    // {
    //     voltageRMSBuffer[sin_index] = inserted_data[1];
    // }
    // if(sin_index >= SIN_TABLE_SIZE)
    // {
    //     //ready to calculate RMS and offset adjustment
    //     fRMSCalculation = READY_TO_CALCULATE;
    // }
    
    // //input AC voltage
    // float inp_voltage_measured = hex_to_volt(inverterInputVoltage) * VOLTAGE_DIVIDER * VOLTAGE_OAMP_GAIN;

        
    // switch (ac_input_state.mains_state)
    // {
    //     case MAINS_STATE_UNKNOWN:
    //     case MAINS_NOT_OK:
    //         if(inp_voltage_measured > 20.0f) ac_input_state.mains_state = MAINS_HV_DETECTED;
    //     break;
    //     case MAINS_HV_DETECTED:
    //         if(inp_voltage_measured < 5.0f) ac_input_state.mains_state = MAINS_NOT_OK;
    //         else if(inp_voltage_measured < 20.0f) ac_input_state.mains_state = MAINS_ZC_DETECTED;
    //     break;
    //     case MAINS_ZC_DETECTED:
    //         if(inp_voltage_measured < 5.0f) ac_input_state.mains_state = MAINS_NOT_OK;
    //         else if(inp_voltage_measured > 20.0f) ac_input_state.mains_state = MAINS_HV_DETECTED;
    //     break;
    //     case MAINS_SYNCHRONIZING:
    //         //wait for stable input voltage and frequency
    //         if(inp_voltage_measured < 5.0f) ac_input_state.mains_state = MAINS_NOT_OK;
    //         else if(ac_input_state.freq_flag == MAINS_FREQENCY_OK) ac_input_state.mains_state = MAINS_OK;
    //     break;
    //     case MAINS_OK:
    //         if(inp_voltage_measured < 5.0f) ac_input_state.mains_state = MAINS_NOT_OK;
    //         else if(ac_input_state.freq_flag == MAINS_FREQUENCY_NOT_OK) ac_input_state.mains_state = MAINS_SYNCHRONIZING;
    //     break;

    // }
    //timer_autoreload_value_config(TIMER0, PWM_MID + 20);
}
        
    

extern uint8_t dma0_ch0_complete;
void DMA0_Channel0_IRQHandler(void)
{
    dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);
    dma0_ch0_complete = 1;
}




uint16_t mains_freq_counter = 0;
/*!
    \brief      this function handles external lines 5 to 9 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    static uint16_t filtered_half_period_us = MAINS_HALF_PERIOD_NOMINAL_US;
    static int32_t phase_integrator = 0;

    if(RESET != exti_interrupt_flag_get(EXTI_7)) {
        uint16_t half_period_us;
        uint16_t full_period_us;
        uint16_t target_inv_phase_us;
        int32_t phase_error_us;
        int32_t freq_adj;
        int32_t phase_trim;
        int32_t arr_adj;
        int32_t half_period_i32;

        if(!gpio_output_bit_get(P29_GPIO_PORT, P29))
            gpio_bit_set(P29_GPIO_PORT, P29);
        else
            gpio_bit_reset(P29_GPIO_PORT, P29);  
            
        // if(polarity == NEGATIVE_HALF_CYCLE) gpio_bit_set(P29_GPIO_PORT, P29);
        // else gpio_bit_reset(P29_GPIO_PORT, P29);     

        exti_interrupt_flag_clear(EXTI_7);
    
        period_timer_stop();
        half_period_us = period_timer_get();
        period_timer_start();
        half_period_us = clamp_u16(half_period_us, MAINS_HALF_PERIOD_MIN_US, MAINS_HALF_PERIOD_MAX_US);
        filtered_half_period_us = (uint16_t)(((uint32_t)filtered_half_period_us * ((1U << PLL_HALF_PERIOD_FILTER_SHIFT) - 1U) + half_period_us) >> PLL_HALF_PERIOD_FILTER_SHIFT);

        full_period_us = (uint16_t)(half_period_us * 2U);
        half_period_i32 = (int32_t)half_period_us;
        ac_input_state.period = full_period_us;
        ac_input_state.freq = 1000000.0f / (float)full_period_us;

        target_inv_phase_us = (uint16_t)(half_period_us - HYST_DELAY);
        phase_error_us = (int32_t)ac_input_state.inv_phase - (int32_t)target_inv_phase_us;

        if(gpio_input_bit_get(GPIOD, GPIO_PIN_7)) {
            ac_input_state.polarity = POSITIVE_HALF_CYCLE;
        } else {
            ac_input_state.polarity = NEGATIVE_HALF_CYCLE;
        }

        if(polarity != ac_input_state.polarity) {
            if(phase_error_us >= 0) {
                phase_error_us -= half_period_i32;
            } else {
                phase_error_us += half_period_i32;
            }
        } else 
        {
        }
        phase_error_us = wrap_phase_error_i32(phase_error_us, full_period_us);
        ac_input_state.phase_delay = phase_error_us;



        if((full_period_us >= (MAINS_PERIOD - MAINS_PERIOD_THR)) &&
           (full_period_us <= (MAINS_PERIOD + MAINS_PERIOD_THR))) {

            if(mains_freq_counter < 10U) {
                mains_freq_counter++;
            }
            if(mains_freq_counter >= 10U) {
                ac_input_state.freq_flag = MAINS_FREQENCY_OK;
            }
        } else {
            mains_freq_counter = 0;
            ac_input_state.freq_flag = MAINS_FREQUENCY_NOT_OK;
            phase_integrator = 0;
        }

        freq_adj = (((int32_t)filtered_half_period_us - (int32_t)MAINS_HALF_PERIOD_NOMINAL_US) * (int32_t)PWM_MID) /
                   (int32_t)MAINS_HALF_PERIOD_NOMINAL_US;

        phase_integrator -= (phase_error_us >> PLL_PHASE_INT_SHIFT);
        phase_integrator = clamp_i32(phase_integrator, -PLL_PHASE_TRIM_LIMIT, PLL_PHASE_TRIM_LIMIT);

        phase_trim = phase_integrator - (phase_error_us >> PLL_PHASE_PROP_SHIFT);
        phase_trim = clamp_i32(phase_trim, -PLL_PHASE_TRIM_LIMIT, PLL_PHASE_TRIM_LIMIT);

        arr_adj = freq_adj + phase_trim;
        arr_adj = clamp_i32(arr_adj, PERIOD_NEGATIVE_ADJ, PERIOD_POSITIVE_ADJ);
        ac_input_state.invPeriodAdj = (int)arr_adj;

        timer_autoreload_value_config(TIMER0, (uint16_t)(PWM_MID + ac_input_state.invPeriodAdj));

        printf("grid:%uHz*100 zerr:%ldus inv:%uus arr:%d\n",
               (uint16_t)(ac_input_state.freq * 100.0f),
               (long)phase_error_us,
               ac_input_state.inv_phase,
               ac_input_state.invPeriodAdj);
    }
}

void TIMER6_IRQHandler(void)
{
    timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
    period_timer_stop();
    period_timer_start();
    ac_input_state.freq_flag = MAINS_FREQUENCY_NOT_OK;
    mains_freq_counter = 0;
}
