/*!
    \file    systick.c
    \brief   the systick configuration file

   \version 2025-7-31, V3.0.2, firmware for GD32F30x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f30x.h"
#include "systick.h"
#include "common.h"


#define SYS_TICK_RELOAD_VAL  ((uint32_t)(SystemCoreClock / 10U))  /* reload value for 100ms interrupts */
int64_t   sysctrl_tick_time_ns = ((1.0 / 120000000) * NS_PER_SEC);


static volatile uint32_t delay;
volatile uint64_t sys_time = 0;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 10Hz interrupts = 100ms */
    if (SysTick_Config(SYS_TICK_RELOAD_VAL)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0FU);  //low priority
}

/*!
    \brief      delay a time in microseconds
    \param[in]  n: delay in microseconds
    \param[out] none
    \retval     none
*/
void Delay_Us(uint32_t n)
{
    uint64_t time_us = get_sys_time_us() + n;
    
    while(get_sys_time_us() < time_us){
    }
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  n: delay in milliseconds
    \param[out] none
    \retval     none
*/
void Delay_Ms(uint32_t n)
{
    uint64_t time_ms = get_sys_time_ms() + n;
  
    while(get_sys_time_ms() < time_ms){
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if (0U != delay){
        delay--;
    }
}

//return system time in microseconds
int64_t get_sys_time_ns(void)
{
    return sys_time * 100 * NS_PER_MS +  (SYS_TICK_RELOAD_VAL - SysTick->VAL) * sysctrl_tick_time_ns;
}


//return system time in microseconds
uint64_t get_sys_time_us(void)
{
    return sys_time * 100 * 1000 + (uint32_t)SysTick->VAL / 120;
}

//return system time in milliseconds
uint32_t get_sys_time_ms(void)
{
    uint64_t time_us = get_sys_time_us();
    return (uint32_t)(time_us / US_PER_MS); 
       
}