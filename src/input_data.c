#include <string.h>

#include "debug.h"
#include "math.h"
#include "app_time.h"

#include "timer.h"
#include "input_data.h"

#include "global.h"

#include "crc16.h"
#include "mei.h"

extern struct pb_msg_t pbReceieved;
extern int ifPanelMsg(void);

int exec_piv_msg(int64_t itime, uint8_t * buf, uint32_t msg_type, uint32_t buf_len);
int read_mei_fifo(int64_t itime);
int read_mei_fifo_from_udp(int64_t itime);
int read_HW_mei_fifo(int64_t itime);
int send_piv_msg(int64_t itime, uint8_t msg_type, uint8_t * msg, uint32_t msg_len);



void periodic_read_mei_buf(int64_t itime)
{
    if(ifPanelMsg())
    {
        read_HW_mei_fifo(itime);
    }
}





int unpack_msg(int64_t itime, uint32_t msg_type, uint8_t * buf, struct pb_msg_t *msg)
{
   uint8_t it = 0;
   //skip msg_type
//    msg->type = (uint8_t)msg_type; it++;
//    msg->updated = buf[it]; it++;   
//    msg->state = (uint16_t)buf[it++]<<8;  it += 2;
//    msg->uMains = (uint16_t)buf[it]; it += 2;
//    msg->uInvertor = (uint16_t)buf[it]; it += 2;
//    msg->iInvertor = (uint16_t)buf[it]; it += 2;
//    msg->uBattery = (uint16_t)buf[it]; it += 2;
//    msg->iBattery = (uint16_t)buf[it];
//    msg->updated = 1;

   msg->type = (uint8_t)msg_type; buf++;
   msg->updated = *buf++;   
   msg->state = *(uint16_t*)buf; buf += 2;
   msg->uMains = *(uint16_t*)buf; buf += 2;
   msg->uInvertor = *(uint16_t*)buf; buf += 2;
   msg->iInvertor = *(uint16_t*)buf; buf += 2;
   msg->uBattery = *(uint16_t*)buf; buf += 2;
   msg->iBattery = *(uint16_t*)buf;
   msg->updated = 1;

   return OK;
}




int exec_piv_msg(int64_t itime, uint8_t * buf, uint32_t msg_type, uint32_t buf_len)
{
  //  struct field_value unpacked_struct[PIV_MSG_MAX_SIZE];
    uint32_t unpacked_struct_len = PIV_MSG_MAX_SIZE;
    uint32_t msg_id = 0xFFFFFFFF;
    static uint16_t calc_crc_recv_file = 0;
    static uint32_t spi_flash_addr = 0;

    //debug_rx_mei = 0;
    if(debug_rx_mei)
    {
        print_time_s_ns(itime);
        printf(" %s(%d) ", __FUNCTION__, __LINE__);
        printf("len %d <", buf_len);
        for(int i = 0; i < (int)buf_len; ++i)
        {
            printf("%02X ", buf[i]);
        }
        printf(">\n");
    }
    

    if(ERR == unpack_msg(itime, msg_type, buf, &pbReceieved))
    {
        return ERR;
    }
    

    if(debug_rx_piv) // печать принятого сообщения ПИВ
    {
        print_time_s_ns(itime);
        printf(" %s(%d) state: %04X, uM: %3.1f, uI: %3.1f, iI: %2.1f, uB: %2.1f, iB: %1.1f\n",
                        __FUNCTION__
                        , __LINE__
                        , pbReceieved.state
                        , pbReceieved.uMains/10.0
                        , pbReceieved.uInvertor/10.0
                        , pbReceieved.iInvertor/10.0
                        , pbReceieved.uBattery/10.0
                        , pbReceieved.iBattery/10.0
                        );
    }
    pbReceieved.updated = 1;
    return OK;
 
}

int send_piv_msg(int64_t itime, uint8_t msg_type, uint8_t * msg, uint32_t msg_len)
{
    
    uint8_t * buf = msg;
    uint32_t len = msg_len;
    uint8_t type = msg_type;

    return send_mei_msg(type, buf, len);
}
