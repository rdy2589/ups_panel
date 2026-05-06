#include <stdio.h>
#include "debug.h"
#include "main.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_usart.h"

#include "systick.h"

//#include "version.h"
//#include "ch32v30x_conf.h"



#ifndef UART_DMA
static char array_data[UART_TX_BUFFER_SIZE];
static char *   p_data_write = array_data;
static char *   p_data_read  = array_data;
static uint32_t arr_size     = UART_TX_BUFFER_SIZE;
#endif

#ifdef SERIAL_PARAMETERS
char rx_array_data[UART_TX_BUFFER_SIZE];
char *pRX_data = rx_array_data;
#endif

#ifdef UART_DMA
    volatile RingBuffer txRing;
     #if(DEBUG ==  DEBUG_UART0)
        //USART_TypeDef *huart = USART0;
        uint32_t huart = USART0;
    #endif
    #if(DEBUG ==  DEBUG_UART1)
        USART_TypeDef *huart = USART1;
    #endif
    #if(DEBUG ==  DEBUG_UART5)
        USART_TypeDef *huart = UART5;
    #endif
#endif

#ifndef UART_DMA
#if(DEBUG ==  DEBUG_UART0)
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
#if(DEBUG ==  DEBUG_UART1)
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
#if(DEBUG ==  DEBUG_UART5)
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
void init_USART_printf(uint32_t baudrate)
{
#ifndef UART_DMA
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    #if(DEBUG == DEBUG_UART1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;

        USART_Init(USART1, &USART_InitStructure);
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);		//Disable TX interrupt
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		//Enable RX interrupt
        USART_Cmd(USART1, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    #elif(DEBUG == DEBUG_UART5)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
/*
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;

        USART_Init(UART5, &USART_InitStructure);
        USART_ITConfig(UART5, USART_IT_TXE, DISABLE);		//Disable TX interrupt
//       USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		//Enable RX interrupt
        USART_Cmd(UART5, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    #endif

    // Clear Minicom console
    num_print(0x1B);
    num_print(0x5B);
    num_print(0x32);
    num_print(0x4A);


#elif defined(UART_DMA)
    #if(DEBUG ==  DEBUG_UART0)
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART0);


        gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

        usart_baudrate_set(USART0, baudrate);
        /* enable USART0 half duplex mode*/
        usart_halfduplex_enable(USART0);
        usart_stop_bit_set(USART0, USART_STB_1BIT);
        usart_word_length_set(USART0, USART_WL_8BIT);
        usart_parity_config(USART0, USART_PM_NONE);
        usart_data_first_config(USART0, USART_MSBF_LSB); //USART_MSBF_LSB

        /* configure USART transmitter */
        usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
        /* configure USART receiver */
        //usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        /* enable USART */
        usart_enable(USART0);

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
    #if(DEBUG ==  DEBUG_UART1)
        GPIO_InitTypeDef  GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure = {0};
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        //Init Rx Pin
        //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        //GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;

        USART_Init(USART1, &USART_InitStructure);
        USART_Cmd(USART1, ENABLE);

        UART_DMA_Init();
        DMA_Interrupt_Init();
    #endif
    #if(DEBUG ==  DEBUG_UART5)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        //Init Rx Pin
        //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        //GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;

        USART_Init(UART5, &USART_InitStructure);
        USART_Cmd(UART5, ENABLE);

        UART_DMA_Init();
        DMA_Interrupt_Init();
    #endif

    txRing.head = 0;
    txRing.tail = 0;
    txRing.state = 0;

#endif
}


/**********************************************************************
 *  @fn      UART_DMA_Init
 *  @brief   initialize uart1 DMA
 *           UART1  TX-->DMA1_Channel4
 *  @param   none
 *  @return  none
 * */
void UART_DMA_Init(void)
{
    //DMA_InitTypeDef DMA_InitStructure = {0};
    dma_parameter_struct DMA_InitStructure;

#if(DEBUG ==  DEBUG_UART0)
    //DMA0 Channel 3
    rcu_periph_clock_enable(RCU_DMA0);
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* USART2  TX-->DMA1_Channel4 */
    dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&DMA_InitStructure);
    //DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.periph_addr = ((uint32_t)&USART_DATA(USART0)); //DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
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
    dma_init(DMA0, DMA_CH3, &DMA_InitStructure);
    
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);


    //DMA_ClearFlag(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_TC4);
    dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_FTF);

    //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    //dma_buf.p_data_write = dma_buf.data; //empty element pointer
    //dma_buf.p_data_read = dma_buf.data;
    //dma_buf.p_data_end = dma_buf.data + UART_DMA_BUF_SIZE -1;
#endif    
#if(DEBUG ==  DEBUG_UART1)

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* USART2  TX-->DMA1_Channel4 */
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    
    DMA_ClearFlag(DMA1_IT_TC4);
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    //dma_buf.p_data_write = dma_buf.data; //empty element pointer
    //dma_buf.p_data_read = dma_buf.data;
    //dma_buf.p_data_end = dma_buf.data + UART_DMA_BUF_SIZE -1;
#endif
#if(DEBUG ==  DEBUG_UART5)

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* USART2  TX-->DMA1_Channel4 */
    DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART5->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    
    DMA_ClearFlag(DMA2_IT_TC4);
    DMA_ClearITPendingBit(DMA2_IT_TC4);
    //USART_DMACmd(USART5, USART_DMAReq_Tx, ENABLE);

    //dma_buf.p_data_write = dma_buf.data; //empty element pointer
    //dma_buf.p_data_read = dma_buf.data;
    //dma_buf.p_data_end = dma_buf.data + UART_DMA_BUF_SIZE -1;
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
void DMA_Interrupt_Init(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure = {0};

#if(DEBUG ==  DEBUG_UART0)
    /* USART0  TX-->DMA0_Channel3*/


    /* enable DMA0 channel3 transfer complete interrupt */
    //DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);

    nvic_irq_enable(DMA0_Channel3_IRQn, 2, 2);

    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	//DMA_ClearITPendingBit(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_GL4);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_FLAG_G);
    //NVIC_Init(&NVIC_InitStructure);
#endif

#if(DEBUG ==  DEBUG_UART1)
    /* USART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5 */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_ClearITPendingBit(DMA1_IT_GL4);
    NVIC_Init(&NVIC_InitStructure);
#endif
#if(DEBUG ==  DEBUG_UART5)
    /* USART5  TX-->DMA2_Channel4   RX-->DMA2_Channel2 */
    DMA_ITConfig(DMA2_Channel4, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	DMA_ClearITPendingBit(DMA2_IT_TC4);
    DMA_ClearITPendingBit(DMA2_IT_GL4);
    NVIC_Init(&NVIC_InitStructure);
#endif
}

#ifndef UART_DMA
uint8_t check_empty_(void)
{
    if (p_data_write == p_data_read)
        return 1;
    else
        return 0;
}

/* Return 1 if full */
uint8_t check_full_(void)
{
    if (p_data_write > p_data_read)
    {
        if (p_data_read + arr_size - p_data_write <= 1)
            return 1;
        else
            return 0;
    }
    else if (p_data_write < p_data_read)
    {
        if (p_data_read - p_data_write <= 1)
            return 1;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}


uint8_t write_qw_(char data)
{
    if (!check_full_())
    {
        *p_data_write = data;
        p_data_write++;
        if (p_data_write >= array_data + arr_size)
            p_data_write = array_data;
        return 1;
    }
    else
    {
        return 0;
    }
}


/* Read byte from queue to data */
uint8_t read_qw_(char * data)
{
    if(!check_empty_())
    {
        *data = *p_data_read;
        p_data_read++;
        if (p_data_read >= array_data + arr_size)
            p_data_read = array_data;
        return 1;
    }
    else
    {
        return 0;
    }
}
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


__attribute__((used)) int _write(int fd, char *buf, int len)
{
#ifndef UART_DMA
    for(int i = 0; i < len; i++)
    {
        #if(DEBUG == DEBUG_UART1)
            while(!write_qw_(*buf));
            USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
            buf++;
        #elif(DEBUG == DEBUG_UART5)
            while(!write_qw_(*buf));
            USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
            buf++;
        #endif
    }
    return len;
#elif defined(UART_DMA)
    for(int i = 0; i < len; i++)
    {
        while(ringBufferPut(&txRing, (uint8_t)buf[i]) != 0){}; // wait until there is space in ring buffer
        //ringBufferPut(&txRing, (uint8_t)buf[i]);    
    }

    uartStartTxDMA(huart, &txRing);
    return len;
#endif

}


/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
/*
__attribute__((used)) void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}
*/
#include "stdarg.h"

int str_print(const char *st)
{
    int i = 0;
    for(i = 0; st[i] != '\0'; i++){	}
        _write(1, (char *)st, i);
    return 0;

}

int str_len(const char *st)
{
    int i = 0;

    for(; st[i] != '\0'; i++){	}
    return i;

}


// Signed 4 byte long num print
// Max 2147483647 min -2147483648
// max 0x7fffffffL min	0xFFFFFFFF
int int32_print(char * begin, int32_t num, uint32_t pos, char * end)
{
    char temp[12];
    char buf[200];
    uint32_t size = 0;	 //size of num positions
    uint32_t i = 0;
    uint8_t sign = 0;

    if(num < 0)
    {
        sign = 1;
        num = -num;
    }

    i = 0;
    do {
        temp[i++] = (char)(num % 10) + '0';
    }
    while(num /= 10);

    for(size = 0; begin[size] != '\0'; size++)
    {
        buf[size] = begin[size];
    }

    if((i+sign) >= pos) pos = i + sign;
    else
        while (pos > (i+sign))
        {
            buf[size++] = ' ';
            pos--;
        }

    if(sign) buf[size++] = '-';

    do {
        buf[size++] = temp[--i];
    }
    while(i);

    for(i = 0; end[i] != '\0'; i++)
    {
        buf[size++] = end[i];
    }
    _write(1, buf, size);

    return 0;
}


int uint32_print(char * begin, uint32_t num, uint32_t pos, char *end)
{
    char temp[12];
    char buf[200];
    uint32_t size = 0;	 //size of num positions
    uint32_t i = 0;

    i = 0;
    do {
        temp[i++] = num % 10 + '0';
    }
    while(num /= 10);

    for(size = 0; begin[size] != '\0'; size++)
    {
        buf[size] = begin[size];
    }

    while (pos > i)
    {
        buf[size++] = '0';
        pos--;
    }

    do {
        buf[size++] = temp[--i];
    }
    while(i);

    for(i = 0; end[i] != '\0'; i++)
    {
        buf[size++] = end[i];
    }
    _write(1, buf, size);

    return 0;
}


int hex_print(char * begin, uint32_t num, uint32_t pos, char * end)
{
    char buf[200];
    uint32_t i = 0;
    char temp[12];
    int32_t size = 0;	 //size of num positions

    i = 0;
    do {
        char digit;
        digit = (num & 0xF);
        if(digit > 9) temp[i++] = (digit - 10) + 'A';
        else temp[i++] = digit + '0';
    }
    while(num >>= 4);
    if(i & 0x01) temp[i++] = '0';

    for(size = 0; begin[size] != '\0'; size++)
    {
        buf[size] = begin[size];
    }

    if(i >= pos) pos = i;
    else
        while (pos > i)
        {
            buf[size++] = '0';
            pos--;
        }

    do {
        buf[size++] = temp[--i];
    }
    while(i);


    for(i = 0; end[i] != '\0'; i++)
    {
        buf[size++] = end[i];
    }
    _write(1, buf, size);

    return 0;
}


int num_print(uint32_t num)
{
    _write(1, (char* )num, 1);

    return 0;
}

void stopstopFuncError(const char * fileName, int fileLine, const char * error)
{
    printf("ERROR: %s(%d), %s\n", fileName, fileLine, error);
    while(1)
    {
        //reset_WDT();
        //Delay_Ms(2000);
        Delay_Ms(1000);
    }
}

void stopFuncError(const char * fileName, int fileLine, const char * error)
{
    printf("ERROR: %s(%d), %s\n", fileName, fileLine, error);
    //Delay_Ms(100);
    Delay_Ms(100);
}



#ifdef UART_DMA
// Добавление байта в буфер
int32_t ringBufferPut(RingBuffer *rb, uint8_t data)
{
    uint16_t next = (rb->head + 1) % UART_TX_BUFFER_SIZE;
    if(next == rb->tail) 
    {
         //DMA improperly shutdown. ENABLE it again
         //DMA_Cmd(DMA1_Channel4, ENABLE);
         //USART_DMACmd(huart, USART_DMAReq_Tx, ENABLE);
         return (-1); // buffer is full
    }
    rb->buffer[rb->head] = data;
    rb->head = next;
    return 0;
}

// Количество байт в буфере
uint16_t ringBufferCount(RingBuffer *rb)
{
    return (UART_TX_BUFFER_SIZE + rb->head - rb->tail) % UART_TX_BUFFER_SIZE;
}

void uartStartTxDMA(uint32_t huart, RingBuffer *rb) {
//void uartStartTxDMA(USART_TypeDef * huart, RingBuffer *rb) {
    //if(USART_GetFlagStatus(huart, USART_FLAG_TXE)  == SET)
    __disable_irq();
    if(rb->state == 1) 
    {   
        __enable_irq();
        return;
    }
    {
        uint16_t len = 0;
        uint16_t tail = rb->tail;

        if(rb->head >= tail)
            len = rb->head - tail;
        else
            len = UART_TX_BUFFER_SIZE - tail;
        
        if(len > 0)
        {
            rb->block_len = len; // save block length
            txRing.state = 1;
            UART_Transmit_DMA(huart, &(rb->buffer[tail]), len);

        }
    }
    __enable_irq();
}

#if(DEBUG ==  DEBUG_UART0)
void UART_Transmit_DMA(uint32_t huart, uint8_t *data, uint16_t size)
{
    //DMA_Cmd(DMA1_Channel4, DISABLE);
    //DMA_ClearITPendingBit(DMA1_IT_TC4);
    //DMA_ClearITPendingBit(DMA1_IT_GL4);
    
    txRing.state = 1; // busy

    dma_channel_disable(DMA0, DMA_CH3);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_FTF);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_G);

    //DMA1_Channel4->MADDR = data;
    //DMA1_Channel4->CNTR = size;
    DMA_CHMADDR(DMA0, DMA_CH3) =  (uint32_t)data;
    DMA_CHCNT(DMA0, DMA_CH3) = (size & DMA_CHANNEL_CNT_MASK);

    //start DMA transfer
    //DMA_Cmd(DMA1_Channel4, ENABLE);
    //USART_DMACmd(huart, USART_DMAReq_Tx, ENABLE);
    dma_channel_enable(DMA0, DMA_CH3);
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    
}

void DMA0_Channel3_IRQHandler(void);
void DMA0_Channel3_IRQHandler(void)
//void DMA1_Channel4_IRQHandler(void)
{
	if (dma_interrupt_flag_get(DMA0, DMA_CH3, DMA_INT_FLAG_FTF))
    //if (DMA_GetITStatus(DMA1_IT_TC4))
	{
        
        //DMA_Cmd(DMA1_Channel4, DISABLE);
        //DMA_ClearITPendingBit(DMA1_IT_TC4);
        //DMA_ClearITPendingBit(DMA1_IT_GL4);
        //USART_ClearITPendingBit(USART1, USART_IT_TC);

        dma_channel_disable(DMA0, DMA_CH3);
        dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);
        dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_FLAG_G);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TC);

        // Отключаем UART DMA Req (чтобы избежать нового запуска пока не настроим)
        //USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
        usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_DISABLE);


        // Move tail after block transmission
        __disable_irq();
        txRing.tail = (txRing.tail + txRing.block_len) % UART_TX_BUFFER_SIZE;
        txRing.block_len = 0;
        txRing.state = 0; // idle
        __enable_irq();
        uartStartTxDMA(huart, &txRing); // send next block
	}    
}
#endif

#if(DEBUG ==  DEBUG_UART1)
void UART_Transmit_DMA(USART_TypeDef *huart, uint8_t *data, uint16_t size)
{
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_ClearITPendingBit(DMA1_IT_GL4);
    
    DMA1_Channel4->MADDR = data;
    DMA1_Channel4->CNTR = size;

    //start DMA transfer
    DMA_Cmd(DMA1_Channel4, ENABLE);
    USART_DMACmd(huart, USART_DMAReq_Tx, ENABLE);
    txRing.state = 1; // busy
}

void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel4_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC4))
	{
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        DMA_ClearITPendingBit(DMA1_IT_GL4);
        USART_ClearITPendingBit(USART1, USART_IT_TC);

        // Отключаем UART DMA Req (чтобы избежать нового запуска пока не настроим)
        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);

        txRing.state = 0; // idle
        //uint16_t len = DMA_GetCurrDataCounter(DMA1_Channel4); // количество оставшихся
        // Move tail after block transmission
        txRing.tail = (txRing.tail + txRing.block_len) % UART_TX_BUFFER_SIZE;
        txRing.block_len = 0;
        uartStartTxDMA(huart, &txRing); // send next block
	}    
}
#endif

#if(DEBUG ==  DEBUG_UART5)
void UART_Transmit_DMA(USART_TypeDef *huart, uint8_t *data, uint16_t size)
{
    DMA_Cmd(DMA2_Channel4, DISABLE);
    DMA_ClearITPendingBit(DMA2_IT_TC4);
    DMA_ClearITPendingBit(DMA2_IT_GL4);
    
    DMA2_Channel4->MADDR = data;
    DMA2_Channel4->CNTR = size;

    //start DMA transfer
    DMA_Cmd(DMA2_Channel4, ENABLE);
    USART_DMACmd(huart, USART_DMAReq_Tx, ENABLE);
    txRing.state = 1; // busy
}

void DMA2_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel4_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_IT_TC4))
	{
        DMA_Cmd(DMA2_Channel4, DISABLE);
        DMA_ClearITPendingBit(DMA2_IT_TC4);
        DMA_ClearITPendingBit(DMA2_IT_GL4);
        USART_ClearITPendingBit(UART5, USART_IT_TC);

        // Отключаем UART DMA Req (чтобы избежать нового запуска пока не настроим)
        USART_DMACmd(UART5, USART_DMAReq_Tx, DISABLE);

        txRing.state = 0; // idle
        //uint16_t len = DMA_GetCurrDataCounter(DMA1_Channel4); // количество оставшихся
        // Move tail after block transmission
        txRing.tail = (txRing.tail + txRing.block_len) % UART_TX_BUFFER_SIZE;
        txRing.block_len = 0;
        uartStartTxDMA(huart, &txRing); // send next block
	}    
}
#endif


#if USE_DEBUG_PIN
    void debug_pin_config(void)
    {
        printf("%s()\n", __func__);
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_GPIOC);
        gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_11);
        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_14);
        gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    }

#endif

#endif