#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>

extern const uint16_t ccitt_16Table[];

uint16_t byte_crc_16    (uint8_t value, uint16_t crcin);
uint16_t byte_crc_16_tab(uint8_t value, uint16_t crcin);
int      ByteCRC16(int value, int crcin);




#endif
