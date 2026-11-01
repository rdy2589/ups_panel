//#include "core_cm4.h"
//#include "system_gd32f30x.h"
#include <stdint.h>
#include "common_type.h"


uint16_t I_out_offset = 0;
uint16_t U_out_offset = 0;
uint16_t U_input_offset = 0;

float U_zero = 0;
float I_zero = 0;

//AC input grid period
uint16_t period = 0;

uint16_t adc_value[4] = {0, 0, 0, 0};

uint8_t dma0_ch0_complete = 0;

struct ac_input_state_t ac_input_state;


uint16_t fRMSCalculation;
int inverterOffsetAdjustment;
int inverterVoltageImbalance;
uint16_t inverterImbalanceCount;

enum mains_quadrant_t mainsQuadrant = UNKNOWN_QUADRANT;
enum system_state_t systemState = SYSTEM_STARTUP;
enum mains_state_t mainsState = MAINS_STATE_UNKNOWN;
enum battery_state_t batteryState = BATTERY_UNKNOWN;
enum dc_link_state_t dcLinkState = DC_LINK_UNKNOWN;
enum error_state_t errorState = NO_FAULT;
enum inverter_current_state_t inverterCurrentState = INVERTER_CURRENT_UNKNOWN;
enum driver_state_t inverterDriverState = DRIVER_FAULT_CLEARED;
enum mains_state_t mainsStartupState = MAINS_STATE_UNKNOWN;
enum relay_state_t fSwitchRelay = READY_TO_SWITCH;

int debug_i2c = 0;
int debug_rx_mei = 1;
int debug_rx_piv = 1;