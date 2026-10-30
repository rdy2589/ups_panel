#include <string.h>
#include <stdio.h>


#include "debug.h"
#include "global.h"
#include "crc16.h"
#include "usart.h"
#include "mei.h"

#include "input_data.h"

extern int exec_piv_msg(int64_t itime, uint8_t * buf, uint32_t buf_len);



int exec_mei_msg(uint8_t * m, uint32_t mi, uint32_t msg_type, uint32_t msg_len, uint32_t crc, int64_t itime);

struct mei_buf_t buf0;
struct mei_buf_t buf1;



uint8_t* put_in_buf(uint8_t* ptr, uint8_t value)
{
    *ptr = value;
    ++ptr;
    return ptr;
}

int send_mei_msg(uint8_t msg_type, uint8_t * msg, uint32_t msg_len)
{
    if(msg == NULL) return 0;
    if(msg_len == 0 || msg_len > PIV_MSG_MAX_SIZE) return 0;
        
    //static variables due DMA transmmission
    static uint8_t serial_buf[PIV_MSG_MAX_SIZE];
    static uint16_t len = 0;
        
    uint16_t crc_tab = 0;
    uint8_t* ptr = serial_buf;

    
    len = 0;
    memset(serial_buf, 0, PIV_MSG_MAX_SIZE);

    ptr = put_in_buf(ptr, 0xAA);
    len++;
    ptr = put_in_buf(ptr, 0x02);
    len++;
    ptr = put_in_buf(ptr, msg_len - 1);
    len++;
    ptr = put_in_buf(ptr, msg_type);
    len++;
    crc_tab = byte_crc_16_tab(msg_type, crc_tab);
    
    if(0) printf("byte 0x%02X crc_tab 0x%04X \n", msg_type, crc_tab);

    for(int i = 0; i < (int)msg_len; ++i)
    {
        if(msg[i] == 0xAA)
        {
            ptr = put_in_buf(ptr, 0xAA);
            ++len;
        }

        ptr = put_in_buf(ptr, msg[i]);
        ++len;

        crc_tab = byte_crc_16_tab(msg[i], crc_tab);

        if(0)
        {
            printf("byte 0x%02X crc_tab 0x%04X \n", msg[i], crc_tab);
        }
    }

    ptr = put_in_buf(ptr, (crc_tab >> 8) & 0xFF);
    ++len;

    if (((crc_tab >> 8) & 0xFF) == 0xAA)
    {
        ptr = put_in_buf(ptr, 0xAA);
        ++len;
    }

    ptr = put_in_buf(ptr, (crc_tab >> 0) & 0xFF);
    ++len;

    if (((crc_tab >> 0) & 0xFF) == 0xAA)
    {
        ptr = put_in_buf(ptr, 0xAA);
        ++len;
    }
    
    ptr = put_in_buf(ptr, 0xAA);
    ptr = put_in_buf(ptr, 0x03);
    len += 2;

    if(panel_to_pb_transmit(serial_buf, len) == ERR) return ERR;
    return OK;
}


int read_HW_mei_fifo(int64_t itime)
{
    uint8_t m[512];

    static int32_t c = 0;

    uint32_t fifo_status;
    uint32_t fifo_rx_word;
    uint32_t msg_type;


    uint8_t  b;
    uint32_t res = 0; // 0 - none; 1 - error;
    static uint32_t msg_state = 0;
    int      ret = 0;

    static uint32_t msg_len   = 0;
    static uint32_t crc       = 0;
    static uint32_t mi        = 0;
    static uint32_t crc1      = 0;
    static uint32_t crc2      = 0;

    struct mei_buf_t  * buf;

    //check rs485 buffers
    if(buf0.filled )
    {
        buf = &buf0;
        // printf("%s() proccess buf0\n", __func__);
    }
    else if(buf1.filled)
    {    
        buf = &buf1;
        // printf("%s() proccess buf1\n", __func__);
    }
    else     
        return NO_RES;


    if (c == 0)
    {
        msg_len   = 0;
        msg_type  = 0;
        crc       = 0;
        mi        = 0;
        crc1      = 0;
        crc2      = 0;
    }


    for(int i = 0; i < buf->len; ++i)
    {
        b = buf->data[i];
        ++c;
    
        res = 0;
        if(debug_rx_mei) printf("#1 i %d b %02X msg_len %d crc %04X crc1 %04X crc2 %04X\n", i, b, msg_len, crc, crc1, crc2);
// 0 - before msg, 1 - aa received, 2 - 02 received, 3 - len  received, 4 - type received, 5 - first byte received, 6 - AA received
        switch(msg_state){
        case 0: { if(b==0xAA) { msg_state = 1; res = 3; break; }                                       msg_state = 0; res = 1; break; }
        case 1: { if(b==0xAA) { msg_state = 0; break; } if(b==0x02) { msg_state = 2; break; }          msg_state = 0; res = 1; break; }
        case 2: {                                                                                      msg_state = 3;          break; }
        case 3: {                                                                                      msg_state = 4;          break; }
        case 4: {                                                                                      msg_state = 5;          break; }
        case 5: { if(b==0xAA) { msg_state = 6; break; }                                                msg_state = 5;          break; }
        case 6: { if(b==0xAA) { msg_state = 5; break; } if(b==0x03) { msg_state = 0; res = 2; break; } msg_state = 0; res = 1; break; }
        }
        if(debug_rx_mei) printf("#2 b %02X msg_len %d crc %04X crc1 %04X crc2 %04X\n", b, msg_len, crc, crc1, crc2);

        if(debug_rx_mei)
        {
            printf("#3 b %02X msg_state %d res %d \n", b, msg_state, res);
        }

        if(res == 0)
        {
            if(debug_rx_mei) { if(msg_state != 0) printf(" %02X", fifo_rx_word); }
        }
        if(res == 1)
        {
            if(debug_rx_mei) { if(msg_state != 0) printf(" %02X#%d", fifo_rx_word, c); }
        }
        if(res == 2)
        {
            if(debug_rx_mei) printf(" %02X > crc %04X msg_len %d\n", fifo_rx_word, crc2, msg_len);
            ret = exec_mei_msg(m, mi, msg_type, msg_len, crc2, itime);
            if(ret == ERR)
            {
                if(debug_rx_mei)
                {
                    print_time_s_ns(itime);
                    printf(" %s() ", __FUNCTION__);
                    printf(" len %d ", (int)(mi - 3));
                    printf("       len+type+mi [ %02X %02X %02X] < ", msg_len, msg_type, mi);
                    for(int i = 0; i < (int)(mi); ++i) printf(" %02X", m[i]);
                    printf(" > ");
                    printf("\n");
                }
            }
            else if (ret == OK)
            {
                if(debug_rx_mei) printf("%s() done\n", __FUNCTION__);
            }
            c = 0;
            buf->filled = 0;
            return ret;
        }
        if(res == 3)
        {
            if(debug_rx_mei)
            {
                print_time_s_ns(itime);
                printf(" %s() < %02X", __FUNCTION__, fifo_rx_word);
            }
        }

        switch(msg_state){
        case 0: msg_len = 0; msg_type = 0; mi = 0;                                                      break;
        case 1:                                                                                         break;
        case 2:                                                                                         break;
        case 3: msg_len  = b; crc2 =    0; crc1 =   0; crc = 0;                                         break;
        case 4: msg_type = b; crc2 = crc1; crc1 = crc; crc = byte_crc_16_tab(b, crc); m[mi++] = b;      break;
        case 5:               crc2 = crc1; crc1 = crc; crc = byte_crc_16_tab(b, crc); m[mi++] = b;      break;
        case 6:                                                                                         break;
        }
        if(0) printf("#4 b %02X msg_len %d crc %04X crc1 %04X crc2 %04X\n", b, msg_len, crc, crc1, crc2);
    }

    buf->filled = 0;
    return ERR; // buffer empty message is incomplete
}


int exec_mei_msg(uint8_t * m, uint32_t mi, uint32_t msg_type, uint32_t msg_len, uint32_t crc, int64_t itime)
{
    uint32_t msg_crc = 0;

    msg_crc = ((uint32_t)m[mi - 2] << 8) | ((uint32_t)m[mi - 1] << 0);

    if(mi - 3 != msg_len + 1)
    {
        print_time_s_ns(itime);
        printf(" %s() ", __func__);
        printf(" $$$ wrong len mi %d ", (int)(mi - 3));
        printf("       len+type < %02X %02X", msg_len, msg_type);
        for(int i = 1; i <= (int)(msg_len) + 1; ++i) printf(" %02X", m[i]);
        printf(" > ");
        printf("$$$\n");
        return ERR;
    }
    else if(crc != msg_crc)
    {
        print_time_s_ns(itime);
        printf(" %s() ", __func__);
        printf("       len+type < %02X %02X", msg_len, msg_type);
        for(int i = 1; i <= (int)(msg_len) + 1; ++i) printf(" %02X", m[i]);
        printf(" > ");
        printf("$$$ wrong crc ");
        printf("msg_crc %04X ", msg_crc);
        printf("crc %04X ", crc);
        printf("$$$\n");
        return ERR;
    }


    int res;
    if(ERR == (res = exec_piv_msg(itime, m, (msg_len + 1))))
    {
        print_time_s_ns(itime);
        printf(" %s() ", __func__);
        printf("       len %02X type %02X body <", msg_len, msg_type);
        for(int i = 1; i <= (int)(msg_len) + 1; ++i) printf(" %02X", m[i]);
        printf(" > ");
        printf("msg_crc %04X ", msg_crc);
        printf("crc %04X ", crc);
        printf("$$$\n");
//        return res;
    }
    return res;
}
