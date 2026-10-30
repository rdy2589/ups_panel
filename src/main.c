#include "gd32f30x.h"
#include "gd32f30x_eval.h"
#include "systick.h"
#include <stdio.h>

#include "timer.h"
#include "timer_impl.h"
#include "app_time.h"

#include "debug.h"
#include "state_machine.h"
#include "common_type.h"
#include "lcd.h"
#include "usart.h"
#include "global.h"

struct pb_msg_t pbReceieved;

void one_sec_task(int64_t itime);

void nvic_config(void)
{
    
    nvic_irq_enable(TIMER0_UP_IRQn, 0, 1);
    nvic_irq_enable(ADC2_IRQn, 0, 0);
    nvic_irq_enable(TIMER6_IRQn, 1, 1);


}

void system_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();

    //init_USART_printf(115200);
    init_USART_printf(250000);
    debug_pin_config(); 

    init_panel_USART(250000);
}


int main(void)
{
    system_init();


    int64_t itime = get_itime();

    reg_task("OneSecTask",     &one_sec_desc,          0 ms,   1000 ms,  0, itime, one_sec_task );
    reg_task("MainStateTask",  &main_state_desc,       0 ms,   1 ms,     0, itime, main_state_task );


        /* turn on P29, turn off led5 */
        //gd_eval_led_on(P29);

        for(int64_t i = 0; ; ++i)
        {
            main_body();
        }



       // Delay_Ms(1000);
        /* turn on P30, turn off P29 */
        //gd_eval_led_on(P30);
       // gd_eval_led_off(P29);
      //  Delay_Ms(1000);

       // printf("abcdef23\n");
        /* turn on led4, turn off P30 */
        //gd_eval_led_on(LED4);
        //gd_eval_led_off(P30);
        //Delay_Ms(1000);
        /* turn on led5, turn off led4 */
        //gd_eval_led_off(LED4);
       // Delay_Ms(1000);

}

extern enum system_state_t systemState;
void one_sec_task(int64_t itime)
{

    //print_cur_itime_();
    int64_t tm = get_sys_time_ns();
    print_time_s_ns(tm);
    printf("%s() [%d]\n", __func__, systemState);
    //printf("%s():\n", __func__);
}
