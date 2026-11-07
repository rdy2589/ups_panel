#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "common_type.h"

#define MLF_MASK        ((uint16_t)(1<<0))
#define BAT_MLF_MASK    ((uint16_t)(1<<1))                                   
#define OVH_MLF_MASK    ((uint16_t)(1<<2))
#define INV_MODE_MASK    ((uint16_t)(1<<3))

void main_state_task(int64_t itime);
uint16_t VectorMax(uint16_t length, const uint16_t *buffer);
uint16_t VectorMin(uint16_t length, const uint16_t *buffer);
void inverterTurnOnRoutine(void);
void batteryChargerTurnOnRoutine(void);
void unpackState(uint16_t state, struct inverter_state_t *inverterState);
void relayProceed(void);
void led_proceed(void);

#endif
