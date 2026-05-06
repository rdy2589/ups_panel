#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__




void main_state_task(int64_t itime);
uint16_t VectorMax(uint16_t length, const uint16_t *buffer);
uint16_t VectorMin(uint16_t length, const uint16_t *buffer);
void inverterTurnOnRoutine(void);
void batteryChargerTurnOnRoutine(void);
void pushpullTurnOnRoutine(void);

#endif
