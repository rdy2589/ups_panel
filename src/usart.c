#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "main.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_usart.h"
#include "gd32f30x_gpio.h"

#include "systick.h"

//#include "version.h"
//#include "ch32v30x_conf.h"




#ifndef PANEL_USART_DMA
static char array_data[UART_TX_BUFFER_SIZE];
static char *   p_data_write = array_data;
static char *   p_data_read  = array_data;
static uint32_t arr_size     = UART_TX_BUFFER_SIZE;
#endif

#ifdef SERIAL_PARAMETERS
char rx_array_data[UART_TX_BUFFER_SIZE];
char *pRX_data = rx_array_data;
#endif

#ifdef PANEL_USART_DMA
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


    #if(UPANEL_USART ==  2)
        static void UART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size);
        static void UART_DMA_Init(void);
        static void DMA_Interrupt_Init(void);
    #endif
#endif

#ifndef PANEL_USART_DMA
#if(UPANEL_USART ==  0)
    void USART1_IRQHandler(void) __attribute__((interrupt("machine")));
    void USART1_IRQHandler(void)
    {
        char read_char;
        char recv_char;

        if (USART_GetITStatus(USART1, USART_IT_TXE))
        {
            if (!check_empty_()) {
                read_qw_(&read_char);
                USART_SendData(USART1, read_char);
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
            }
            else if (check_full_()) {
                read_qw_(&read_char);
                USART_SendData(USART1, read_char);
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
            }
            else {
                USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            }
            USART_ClearITPendingBit(USART1, USART_IT_TXE);
        }

    }
#endif
#if(UPANEL_USART ==  1)
    void USART1_IRQHandler(void) __attribute__((interrupt("machine")));
    void USART1_IRQHandler(void)
    {
        char read_char;
        char recv_char;

        if (USART_GetITStatus(USART1, USART_IT_TXE))
        {
            if (!check_empty_()) {
                read_qw_(&read_char);
                USART_SendData(USART1, read_char);
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
            }
            else if (check_full_()) {
                read_qw_(&read_char);
                USART_SendData(USART1, read_char);
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
            }
            else {
                USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            }
            USART_ClearITPendingBit(USART1, USART_IT_TXE);
        }

    }
#endif
#if(UPANEL_USART ==  5)
    void UART5_IRQHandler(void) __attribute__((interrupt("machine")));
    void UART5_IRQHandler(void)
    {
        char read_char;
        char recv_char;

        if (USART_GetITStatus(UART5, USART_IT_TXE))
        {
            if (!check_empty_()) {
                read_qw_(&read_char);
                USART_SendData(UART5, read_char);
                USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
            }
            else if (check_full_()) {
                read_qw_(&read_char);
                USART_SendData(UART5, read_char);
                USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
            }
            else {
                USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
            }
            USART_ClearITPendingBit(UART5, USART_IT_TXE);
        }

    }
#endif
#endif

/*
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct)
{
    NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelSubPriority << 4);

    if(NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)  NVIC_EnableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
    else                                                NVIC_DisableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
}
*/
void init_panel_USART(uint32_t baudrate)
{
#ifndef PANEL_USART_DMA
    #if(UPANEL_USART == 1)
    
    #elif(UPANEL_USART == 2)
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART2);


        gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
        gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

        usart_baudrate_set(USART2, baudrate);
        /* enable USART0 half duplex mode*/
        //usart_halfduplex_enable(USART2);
        usart_stop_bit_set(USART0, USART_STB_1BIT);
        usart_word_length_set(USART0, USART_WL_8BIT);
        usart_parity_config(USART0, USART_PM_NONE);
        usart_data_first_config(USART0, USART_MSBF_LSB); //USART_MSBF_LSB

        /* configure USART transmitter */
        usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
        /* configure USART receiver */
        //usart_receive_config(USART2, USART_RECEIVE_ENABLE);
        /* enable USART */
        usart_enable(USART2);
        // NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
        // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        // NVIC_Init(&NVIC_InitStructure);
    #endif


#elif defined(PANEL_USART_DMA)
    #if(UPANEL_USART ==  2)
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART2);


        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
        gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

        usart_baudrate_set(USART2, baudrate);
        /* enable USART0 half duplex mode*/
        //usart_halfduplex_enable(USART2);
        usart_stop_bit_set(USART2, USART_STB_1BIT);
        usart_word_length_set(USART2, USART_WL_8BIT);
        usart_parity_config(USART2, USART_PM_NONE);
        usart_data_first_config(USART2, USART_MSBF_LSB); //USART_MSBF_LSB

        /* configure USART transmitter */
        usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
        /* configure USART receiver */
        //usart_receive_config(USART2, USART_RECEIVE_ENABLE);
        /* enable USART */
        usart_enable(USART2);

        //Init Rx Pin
        //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        //GPIO_Init(GPIOA, &GPIO_InitStructure);

        //USART_InitStructure.USART_BaudRate = baudrate;
        //USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        //USART_InitStructure.USART_StopBits = USART_StopBits_1;
        //USART_InitStructure.USART_Parity = USART_Parity_No;
        //USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        //USART_InitStructure.USART_Mode = USART_Mode_Tx;

        //USART_Init(USART0, &USART_InitStructure);
        //USART_Cmd(USART0, ENABLE);

        UART_DMA_Init();
        DMA_Interrupt_Init();
    #endif

    txBuf.head = 0;
    txBuf.tail = 0;
    txBuf.state = 0;

#endif
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
    //DMA_InitTypeDef DMA_InitStructure = {0};
    dma_parameter_struct DMA_InitStructure;

#if(UPANEL_USART ==  2)
    //DMA0 Channel 3
    rcu_periph_clock_enable(RCU_DMA0);
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* USART2  TX-->DMA1_Channel4 */
    dma_deinit(DMA0, DMA_CH1);
    dma_struct_para_init(&DMA_InitStructure);
    //DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.periph_addr = ((uint32_t)&USART_DATA(USART2)); //DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
    DMA_InitStructure.memory_addr = 0; //DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.direction = DMA_MEMORY_TO_PERIPHERAL; //DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.number = 4; //DMA_BufferSize = 4;
    DMA_InitStructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE; //DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE; //DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_8BIT; //DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_8BIT; //DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.priority = DMA_PRIORITY_ULTRA_HIGH; //DMA_Priority = DMA_Priority_VeryHigh;
    //DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    //DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    dma_init(DMA0, DMA_CH1, &DMA_InitStructure);
    
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);


    //DMA_ClearFlag(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_TC4);
    dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INT_FLAG_FTF);

    //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    //dma_buf.p_data_write = dma_buf.data; //empty element pointer
    //dma_buf.p_data_read = dma_buf.data;
    //dma_buf.p_data_end = dma_buf.data + UART_DMA_BUF_SIZE -1;
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
    //NVIC_InitTypeDef NVIC_InitStructure = {0};

#if(UPANEL_USART ==  0)
    /* USART0  TX-->DMA0_Channel3*/


    /* enable DMA0 channel3 transfer complete interrupt */
    //DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);

    nvic_irq_enable(DMA0_Channel3_IRQn, 2, 2);

    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	//DMA_ClearITPendingBit(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_GL4);
    dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_G);
    //NVIC_Init(&NVIC_InitStructure);
#endif

#if(UPANEL_USART ==  2)
    /* USART0  TX-->DMA0_Channel3*/


    /* enable DMA0 channel3 transfer complete interrupt */
    //DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);

    nvic_irq_enable(DMA0_Channel1_IRQn, 2, 2);

    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	//DMA_ClearITPendingBit(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_GL4);
    dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_FLAG_G);
    //NVIC_Init(&NVIC_InitStructure);
#endif
}

#ifndef PANEL_USART_DMA
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




#ifdef PANEL_USART_DMA
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
#ifndef PANEL_USART_DMA
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
#elif defined(PANEL_USART_DMA)
    if (txBuf.state == BUSY) {
        printf("USART is busy\n");
        return 1;
    }
    memcpy(txBuf.buffer, buf, len);
    txBuf.block_len = len;
    UART_Transmit_DMA(huart, txBuf.buffer, txBuf.block_len);
    printf("DMA transfer start, size: %d\n", txBuf.block_len);

    return 0;
#endif

}