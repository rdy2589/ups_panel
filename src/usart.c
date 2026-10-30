#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "main.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_usart.h"
#include "gd32f30x_gpio.h"

#include "systick.h"

#include "mei.h"


uint8_t rxbuffer[256];
uint8_t receive_flag = 0;
uint16_t rx_count = 0;

#define USART1_RDATA_ADDRESS      ((uint32_t)&USART_DATA(USART1))

#if PANEL_USART_TX_DMA == 0
static char array_data[UART_TX_BUFFER_SIZE];
static char *   p_data_write = array_data;
static char *   p_data_read  = array_data;
static uint32_t arr_size     = UART_TX_BUFFER_SIZE;
#endif

#ifdef SERIAL_PARAMETERS
char rx_array_data[UART_TX_BUFFER_SIZE];
char *pRX_data = rx_array_data;
#endif

extern struct mei_buf_t buf0;
extern struct mei_buf_t buf1;


volatile Buffer_t txBuf;
#if(UPANEL_USART ==  0)
    uint32_t huart = USART0;
#endif
#if(UPANEL_USART ==  1)
    uint32_t huart = USART1;
#endif
#if(UPANEL_USART ==  5)
    uint32_t huart = USART5;
#endif
#if(UPANEL_USART ==  2)
    static uint32_t huart = USART2;
#endif

#ifdef PANEL_USART_TX_DMA
    static void UART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size);
    static void UART_DMA_Init(void);
    static void DMA_Interrupt_Init(void);
#endif

#if PANEL_USART_TX_DMA == 0
    #if(UPANEL_USART ==  0)

    #endif
    #if(UPANEL_USART ==  1)

    #endif
    #if(UPANEL_USART ==  5)

    #endif
#endif
#if PANEL_USART_TX_DMA == 1
    #if(UPANEL_USART ==  1) 


            void USART1_IRQHandler(void);
            void USART1_IRQHandler(void)
            {
                if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)){
                    /* clear IDLE flag */
                    usart_data_receive(USART1);
                    
                    /* number of data received */
                    rx_count = 256 - (dma_transfer_number_get(DMA0, DMA_CH5));
                    
                    if((!buf0.filled) && (!buf0.mutex))
                    {
                        buf0.mutex = 1;
                        memcpy(buf0.data, rxbuffer, rx_count);
                        buf0.len = rx_count;
                        buf0.filled = 1;
                        buf0.mutex = 0;
                        receive_flag = 1;
                    } else if((!buf1.filled) && (!buf1.mutex))
                    {
                        buf1.mutex = 1;
                        memcpy(buf1.data, rxbuffer, rx_count);
                        buf0.len = rx_count;
                        buf1.filled = 1;
                        buf1.mutex = 0;
                        receive_flag = 1;
                    }
                    
                    /* disable DMA and reconfigure */
                    dma_channel_disable(DMA0, DMA_CH5);
                    dma_transfer_number_config(DMA0, DMA_CH5, 256);
                    dma_channel_enable(DMA0, DMA_CH5);
                }
            }

    #endif
#endif


void init_panel_USART(uint32_t baudrate)
{

    #if(UPANEL_USART == 1)
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART1);

        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
        usart_baudrate_set(USART1, baudrate);
        /* enable USART1 full duplex mode*/
        usart_stop_bit_set(USART1, USART_STB_1BIT);
        usart_word_length_set(USART1, USART_WL_8BIT);
        usart_parity_config(USART1, USART_PM_NONE);
        usart_data_first_config(USART1, USART_MSBF_LSB); //USART_MSBF_LSB

        /* configure USART transmitter */
        usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
        /* configure USART receiver */
        usart_receive_config(USART1, USART_RECEIVE_ENABLE);

        #if (PANEL_USART_TX_DMA == 1)
            UART_DMA_Init();
            DMA_Interrupt_Init();
        #endif
        #if PANEL_USART_RX_DMA == 1
            usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);
            usart_interrupt_enable(USART1, USART_INT_IDLE);
            nvic_irq_enable(USART1_IRQn, 0, 0);
        #endif
        /* enable USART */
        usart_enable(USART1);

    #elif(UPANEL_USART == 2)

    #endif

    #if(UPANEL_USART ==  2)
    #endif

    txBuf.head = 0;
    txBuf.tail = 0;
    txBuf.state = 0;

}


/**********************************************************************
 *  @fn      UART_DMA_Init
 *  @brief   initialize uart1 DMA
 *           UART2  TX-->DMA1_Channel4
 *  @param   none
 *  @return  none
 * */
static void UART_DMA_Init(void)
{
    dma_parameter_struct DMA_InitStructure;
#if(UPANEL_USART ==  1)
    //TX DMA
    #if PANEL_USART_TX_DMA == 1
        //DMA0 Channel 3
        rcu_periph_clock_enable(RCU_DMA0);
        //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        /* USART2  TX-->DMA1_Channel4 */
        dma_deinit(DMA0, DMA_CH6);
        dma_struct_para_init(&DMA_InitStructure);
        //DMA_DeInit(DMA1_Channel4);
        DMA_InitStructure.periph_addr = ((uint32_t)&USART_DATA(USART1)); //DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
        DMA_InitStructure.memory_addr = 0; //DMA_MemoryBaseAddr = 0;
        DMA_InitStructure.direction = DMA_MEMORY_TO_PERIPHERAL; //DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.number = 4; //DMA_BufferSize = 4;
        DMA_InitStructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE; //DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE; //DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_8BIT; //DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_8BIT; //DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.priority = DMA_PRIORITY_ULTRA_HIGH; //DMA_Priority = DMA_Priority_VeryHigh;

        dma_init(DMA0, DMA_CH6, &DMA_InitStructure);
        
        dma_circulation_disable(DMA0, DMA_CH6);
        dma_memory_to_memory_disable(DMA0, DMA_CH6);

        dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_FTF);
    #endif

    //RX DMA
    #if PANEL_USART_RX_DMA == 1
        memset(&DMA_InitStructure, 0, sizeof(dma_parameter_struct));

        rcu_periph_clock_enable(RCU_DMA0);
        
        /* deinitialize DMA channel4 (USART0 rx) */
        dma_deinit(DMA0, DMA_CH5);
        DMA_InitStructure.direction = DMA_PERIPHERAL_TO_MEMORY;
        DMA_InitStructure.memory_addr = (uint32_t)rxbuffer;
        DMA_InitStructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_8BIT;
        DMA_InitStructure.number = 256;
        DMA_InitStructure.periph_addr = USART1_RDATA_ADDRESS;
        DMA_InitStructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        DMA_InitStructure.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(DMA0, DMA_CH5, &DMA_InitStructure);
        /* configure DMA mode */
        dma_circulation_disable(DMA0, DMA_CH5);
        /* enable DMA channel4 */
        dma_channel_enable(DMA0, DMA_CH5);
    #endif
#endif  
#if(UPANEL_USART ==  2)

#endif    
#if(UPANEL_USART ==  3)

#endif
}


/**********************************************************************
 *  @fn      DMA_Interrupt_Init
 *  @brief   initialize usart1 interrupt
 *           TX -- enable  DMA_IT_TC
 *           RX -- enable  DMA_IT_HT
 *           enable  USARTx_IRQn-->USART_IT_IDLE
 *  @param   none
 *  @return  none
 * */
static void DMA_Interrupt_Init(void)
{
    #if(UPANEL_USART ==  0)

    #endif
    #if(UPANEL_USART ==  1)
        /* USART1  TX-->DMA0_Channel6*/

        /* enable DMA0 channel6 transfer complete interrupt */
        dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);

        nvic_irq_enable(DMA0_Channel6_IRQn, 2, 2);

        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_FLAG_G);
    #endif
    #if(UPANEL_USART ==  2)

    #endif
}

#ifndef PANEL_USART_TX_DMA
#endif

#ifdef SERIAL_PARAMETERS
    uint8_t save_rx(char data)
    {
        *pRX_data = data;
        pRX_data++;
        if (pRX_data >= rx_array_data + arr_size)
            pRX_data = rx_array_data;

        return 1;
    }
#endif



#ifdef PANEL_USART_TX_DMA
    #if(UPANEL_USART ==  1)
        #if PANEL_USART_TX_DMA == 1
            static void UART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size)
            {
                txBuf.state = BUSY; // busy

                dma_channel_disable(DMA0, DMA_CH6);
                dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_FTF);
                dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_G);

                DMA_CHMADDR(DMA0, DMA_CH6) =  (uint32_t)data;
                DMA_CHCNT(DMA0, DMA_CH6) = (size & DMA_CHANNEL_CNT_MASK);

                dma_channel_enable(DMA0, DMA_CH6);
                usart_dma_transmit_config(huart, USART_TRANSMIT_DMA_ENABLE);
                
            }

            void DMA0_Channel6_IRQHandler(void);
            void DMA0_Channel6_IRQHandler(void)
            {
                if (dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INT_FLAG_FTF))
                //if (DMA_GetITStatus(DMA1_IT_TC4))
                {
                    dma_channel_disable(DMA0, DMA_CH6);
                    dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);
                    dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_FLAG_G);
                    usart_interrupt_flag_clear(USART1, USART_INT_FLAG_TC);

                    // Отключаем UART DMA Req (чтобы избежать нового запуска пока не настроим)
                    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_DISABLE);

                    txBuf.state = IDDLE; // idle
                    printf("DMA transfer complete, size: %d\n", txBuf.block_len);


                }    
            }
        #endif
    #endif

    #if(UPANEL_USART ==  2)
        static void UART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size)
        {
            //DMA_Cmd(DMA1_Channel4, DISABLE);
            //DMA_ClearITPendingBit(DMA1_IT_TC4);
            //DMA_ClearITPendingBit(DMA1_IT_GL4);
            
            txBuf.state = BUSY; // busy

            dma_channel_disable(DMA0, DMA_CH1);
            dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INT_FLAG_FTF);
            dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INT_FLAG_G);

            DMA_CHMADDR(DMA0, DMA_CH1) =  (uint32_t)data;
            DMA_CHCNT(DMA0, DMA_CH1) = (size & DMA_CHANNEL_CNT_MASK);

            //start DMA transfer
            //DMA_Cmd(DMA1_Channel4, ENABLE);
            //USART_DMACmd(huart, USART_DMAReq_Tx, ENABLE);
            dma_channel_enable(DMA0, DMA_CH1);
            usart_dma_transmit_config(huart, USART_TRANSMIT_DMA_ENABLE);
            
        }

        void DMA0_Channel1_IRQHandler(void);
        void DMA0_Channel1_IRQHandler(void)
        {
            if (dma_interrupt_flag_get(DMA0, DMA_CH1, DMA_INT_FLAG_FTF))
            //if (DMA_GetITStatus(DMA1_IT_TC4))
            {
                dma_channel_disable(DMA0, DMA_CH1);
                dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
                dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_G);
                usart_interrupt_flag_clear(USART2, USART_INT_FLAG_TC);

                // Отключаем UART DMA Req (чтобы избежать нового запуска пока не настроим)
                usart_dma_transmit_config(USART2, USART_TRANSMIT_DMA_DISABLE);

                txBuf.state = IDDLE; // idle
                printf("DMA transfer complete, size: %d\n", txBuf.block_len);

            }    
        }
    #endif
#endif

uint8_t panel_to_pb_transmit(uint8_t* buf, uint16_t len)
{
    #if PANEL_USART_TX_DMA == 0
        for(int i = 0; i < len; i++)
        {
            #if(UPANEL_USART == 1)
                while(!write_qw_(*buf));
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
                buf++;
            #elif(UPANEL_USART == 5)
                while(!write_qw_(*buf));
                USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
                buf++;
            #endif
        }
        return len;
    #endif
    #if PANEL_USART_TX_DMA == 1
        if (txBuf.state == BUSY) {
            printf("USART is busy\n");
            return 1; //ERR
        }
        memcpy(txBuf.buffer, buf, len);
        txBuf.block_len = len;
        UART_Transmit_DMA(huart, txBuf.buffer, txBuf.block_len);
        printf("DMA transfer start, size: %d\n", txBuf.block_len);

        return 0; //OK
    #endif

}


void panel_receieve(void)
{
    #if PANEL_USART_RX_DMA == 0
        for(int i = 0; i < len; i++)
        {
            #if(UPANEL_USART == 1)
                while(!write_qw_(*buf));
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
                buf++;
            #elif(UPANEL_USART == 5)
                while(!write_qw_(*buf));
                USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
                buf++;
            #endif
        }
        return len;
    #elif (PANEL_USART_RX_DMA == 1)
        if(1 == receive_flag) {
            printf("DMA receieve size: %d [%s]\n", rx_count, rxbuffer);
            receive_flag = 0;
            //decode mei protocol
        }
    #endif
}
//return: 1 - message receieved 
int ifPanelMsg(void)
{
    if(1 == receive_flag) {
        //printf("DMA receieve size: %d [%s]\n", rx_count, rxbuffer);
        receive_flag = 0;
        return 1;
    }
    return 0;
}