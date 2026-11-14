#ifndef __MEI_H__
#define __MEI_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"

#define PIV_MSG_MAX_SIZE    ((uint16_t)256)


struct mei_buf_t {
    uint8_t mutex;
    uint8_t filled;
    uint8_t len;
    uint8_t data[PIV_MSG_MAX_SIZE];
};


uint32_t send_data_serial(uint32_t dev, uint32_t data);
int send_mei_msg(uint8_t msg_type, uint8_t * msg, uint32_t msg_len);
int send_mei_msg_hardware(uint8_t msg_type, uint8_t * msg, uint32_t msg_len);
uint8_t panel_to_pb_transmit(uint8_t* buf, uint16_t len);

#endif //MEI_H

