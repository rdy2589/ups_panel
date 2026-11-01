#ifndef __INPUT_DATA_H__
#define __INPUT_DATA_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"

//extern void reset_FPGA(void);

//void periodic_read_mei_buf(int64_t itime);

//void periodic_read_mei_fifo(int64_t itime);
//void periodic_send_int_mei(int64_t itime);
//void periodic_send_test_mei(int64_t itime);


int send_piv_msg(int64_t itime, uint8_t msg_type, uint8_t * msg,  uint32_t msg_len);

#endif //INPUT_DATA_H

