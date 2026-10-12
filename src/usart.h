#ifndef _USART_H_
#define _USART_H_

#include <stdint.h>
#include "main.h"
#include "gd32f30x_usart.h"


//printf via DMA
#define PANEL_USART_TX_DMA             1       //define if DMA used
#define PANEL_USART_RX_DMA            1       //define if DMA used
#define UPANEL_USART                  1       //USART1


#define UPANEL_USART_BUF_SIZE     1024

extern char rx_array_data[UPANEL_USART_BUF_SIZE];
extern char *pRX_data;


enum {
    IDDLE = 0,
    BUSY = 1
};

#ifdef PANEL_USART_TX_DMA
typedef struct {
    uint8_t buffer[UPANEL_USART_BUF_SIZE];
    volatile uint16_t head; // куда пишем
    volatile uint16_t tail; // откуда читаем
    volatile uint8_t state; // 0 - idle, 1 - busy
    volatile uint16_t block_len; // length of current block
    volatile uint16_t block_len1;
} Buffer_t;

uint8_t panel_to_pb_transmit(uint8_t* buf, uint16_t len);
void init_panel_USART(uint32_t baudrate);
void panel_receieve(void);

#endif
#endif // _DEBUG_H_



