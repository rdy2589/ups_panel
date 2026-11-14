#ifndef _DEBUG_H_
#define _DEBUG_H_


//#include "ch32v30x.h"
//#include "system_ch32v30x.h"
//#include "version.h"
#include "main.h"
#include <stdint.h>

//printf via DMA
#define UART_DMA 1
#define DEBUG_UART0 0
#define DEBUG_UART1 1
#define DEBUG_UART2 2
#define DEBUG_UART3 3
#define DEBUG_UART5 4
#define DEBUG_UART6 5
#define DEBUG   DEBUG_UART0

//test points
#define USE_DEBUG_PIN   1
#if USE_DEBUG_PIN
    #define P29                             GPIO_PIN_8
    #define P29_GPIO_PORT                   GPIOA
    #define P29_GPIO_CLK                    RCU_GPIOA
    
    #define P30                             GPIO_PIN_11
    #define P30_GPIO_PORT                   GPIOA
    #define P30_GPIO_CLK                    RCU_GPIOA

    #define P25                             GPIO_PIN_4
    #define P25_GPIO_PORT                   GPIOB
    #define P25_GPIO_CLK                    RCU_GPIOB

    #define P26                             GPIO_PIN_6
    #define P26_GPIO_PORT                   GPIOB
    #define P26_GPIO_CLK                    RCU_GPIOB

    #define P36                             GPIO_PIN_14
    #define P36_GPIO_PORT                   GPIOB
    #define P36_GPIO_CLK                    RCU_GPIOB

    #define P44                             GPIO_PIN_12
    #define P44_GPIO_PORT                   GPIOC
    #define P44_GPIO_CLK                    RCU_GPIOC

#endif

#define UART_TX_BUFFER_SIZE 1024

extern char rx_array_data[UART_TX_BUFFER_SIZE];
extern char *pRX_data;



uint8_t read_qw_(char * data);
uint8_t check_empty_(void);
uint8_t check_full_(void);
void init_USART_printf(uint32_t baudrate);

int str_print(const char * str);
int num_print(uint32_t num);
int str_len(const char *st);
int int32_print(char *begin, int32_t num, uint32_t pos, char *end);
int uint32_print(char *begin, uint32_t num, uint32_t pos, char *end);
int hex_print(char *begin, uint32_t num, uint32_t pos, char *end);
void stopstopFuncError(const char * fileName, int fileLine, const char * error);
void stopFuncError(const char * fileName, int fileLine, const char * error);



#ifdef UART_DMA
typedef struct {
    uint8_t buffer[UART_TX_BUFFER_SIZE];
    volatile uint16_t head; // куда пишем
    volatile uint16_t tail; // откуда читаем
    volatile uint8_t state; // 0 - idle, 1 - busy
    volatile uint16_t block_len; // length of current block
    volatile uint16_t block_len1;
} RingBuffer;

void uartStartTxDMA(uint32_t huart, RingBuffer *rb);
void USART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size);
int32_t ringBufferPut(RingBuffer *rb, uint8_t data);
void UART_DMA_Init(void);
void DMA_Interrupt_Init(void);
void debug_pin_config(void);

#endif
#endif // _DEBUG_H_



