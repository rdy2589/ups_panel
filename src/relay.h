#ifndef __RELAY_H__
#define __RELAY_H__

void inpit_relay_enable(void);
void input_relay_disable(void);
void charge_relay_enable(void);
void charge_relay_disable(void);
void relay_config(void);
void sound_config(void);
void sound_(uint16_t state);

#endif
