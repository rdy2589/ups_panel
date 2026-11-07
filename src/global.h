#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>
#include "common_type.h"

extern uint16_t I_out_offset;
extern uint16_t U_out_offset;
extern uint16_t U_input_offset;
extern float U_zero;
extern float I_zero;
extern uint16_t period;
extern uint32_t adc_value[];

extern uint8_t dma0_ch0_complete;

extern struct ac_input_state_t ac_input_state;


extern uint16_t fRMSCalculation;
extern int inverterOffsetAdjustment;
extern int inverterVoltageImbalance;
extern uint16_t inverterImbalanceCount;

extern enum mains_quadrant_t mainsQuadrant;
extern enum system_state_t systemState;
extern enum mains_state_t mainsState;
extern enum battery_state_t batteryState;
extern enum dc_link_state_t dcLinkState;
extern enum error_state_t errorState;
extern enum inverter_current_state_t inverterCurrentState;
extern enum driver_state_t inverterDriverState;
extern enum mains_state_t mainsStartupState;
extern enum relay_state_t fSwitchRelay;


/* RMS Calculation constants */
enum{
    RMS_BUFFER_SIZE = 128,
    READY_TO_COLLECT_DATA = 0,
    READY_TO_CALCULATE = 1,
    CALCULATION_DONE = 2
};


enum {
    OK = 0,
    ERR,
    NO_RES,
};


extern struct inverter_state_t inverterState;

extern int debug_i2c;
extern int debug_rx_mei;
extern int debug_rx_piv;

#endif
