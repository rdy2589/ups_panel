#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

#include <stdint.h>

enum fault_t
{
    FAULT = 0,
    NORM = 1
};

enum freq_flag_t
{
    MAINS_FREQENCY_OK = 0,
    MAINS_FREQUENCY_NOT_OK = 1
};

/* AC Mains State definitions */
enum mains_state_t
{
    MAINS_STATE_UNKNOWN = 0,
    MAINS_NOT_OK = 1,
    MAINS_HV_DETECTED = 2,
    MAINS_ZC_DETECTED = 3,
    MAINS_SYNCHRONIZING = 4,
    MAINS_OK = 5
};

enum mains_quadrant_t
{
    UNKNOWN_QUADRANT = 0,
    FIRST_QUADRANT = 1,
    SECOND_QUADRANT = 2,
    THIRD_QUADRANT = 3,
    FOURTH_QUADRANT = 4
};

enum system_state_t
{
    SYSTEM_STARTUP = 0,
    INVERTER_MODE = 1,
    WAIT_MODE = 2,
    BATTERY_CHARGER_MODE = 3,
    SYSTEM_ERROR = 4,
    DEBUG_MODE = 5
};

enum battery_state_t
{
    BATTERY_UNKNOWN = 0,
    BATTERY_UNDERVOLTAGE = 1,
    BATTERY_OK = 2,
    BATTERY_OVERVOLTAGE = 3
};

enum dc_link_state_t
{
    DC_LINK_UNKNOWN = 0,
    DC_LINK_UNDERVOLTAGE = 1,
    DC_LINK_OK = 2,
    DC_LINK_OVERVOLTAGE = 3
};

enum error_state_t
{
    NO_FAULT = 0,
    DC_LINK_UNDERVOLTAGE_FAULT = 1,
    DC_LINK_OVERVOLTAGE_FAULT = 2,
    BATTERY_UNDERVOLTAGE_FAULT = 3,
    BATTERY_OVERVOLTAGE_FAULT = 4,
    PUSHPULL_OVERCURRENT_FAULT = 5,
    INVERTER_OVERCURRENT_FAULT = 6,
    INVERTER_DRIVER_FAULT = 7,
    MAINS_STARTUP_FAULT = 8,
    MAINS_FREQUENCY_ERROR = 9
};

enum inverter_current_state_t
{
    INVERTER_CURRENT_UNKNOWN = 0,
    INVERTER_CURRENT_OK = 1,
    INVERTER_OVERCURRENT = 2
};

enum driver_state_t
{
    DRIVER_FAULT_CLEARED = 0,
    DRIVER_FAULT_ACTIVE = 1
};

enum polarity_t
{
    NEGATIVE_HALF_CYCLE = 0,
    POSITIVE_HALF_CYCLE = 1
};

enum relay_state_t
{
    NOT_READY_TO_SWITCH = 0,
    READY_TO_SWITCH = 1,
};

struct ac_input_state_t
{
    enum fault_t fault_flag;
    enum freq_flag_t freq_flag;
    enum mains_state_t mains_state; 
    uint16_t period;            //us
    float freq;                 //hz
    float voltage_f;            //volt
    uint32_t voltage;           //volt^2
    uint16_t inv_phase;
    int phase_delay;       //phase delay between ac input and invertor: (inv_ph+hyst_delay) == (mains_period/2)
    enum polarity_t polarity;          //0: positive half cycle, 1: negative half cycle
    int invPeriodAdj;
};

enum inv_mode_t {
    INV_MODE_UNKNOWN = 0,
    INV_MODE_LINE,
    INV_MODE_BATTERY
};

enum batt_t {
    BATT_UNKNOWN = 0,
    BATT_NOT_OK,
    BATT_OK,
    BATT_LOW,
    BATT_HIGH,
};

enum mlf_t {
    MLF_UNKNOWN = 0,
    MLF_NOT_OK,
    MLF_OK
};

struct inverter_state_t {
    uint8_t updated;
    enum inv_mode_t mode;
    enum batt_t battery;
    enum mlf_t mlf;         //common mallfunction
    enum mlf_t mlf_ovh;     //overheat malfunction
    enum mlf_t mlf_bat;     //battery malfunction
};


struct pb_msg_t {
    uint8_t updated;
    uint8_t type;
    uint16_t state;     //replace with separated values later
    uint16_t uMains;
    uint16_t uInvertor;
    uint16_t iInvertor;
    uint16_t uBattery;
    uint16_t iBattery;
};



#endif
