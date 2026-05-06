#ifndef _TIMER_APP_H_
#define _TIMER_APP_H_

#include <stdio.h>
#include <stdint.h>
// #include <time.h>
// #include <sys/time.h>

#include "common.h"

#define TASK_NAME_LEN 10
struct periodic_task_desc
{
    int64_t                     base_ns;
    int64_t                     start_ns;       // время срабатывания
    int64_t                     period_ns;      // период работы таймера
    int64_t                     span_ns;        // случайный диапазон разброса таймера
    int64_t                     total_ns;
    int64_t                     min_ns;
    int64_t                     max_ns;
    int64_t                     count;
    uint16_t                    divider;
    uint16_t                    half_span_ms;

    void                      (*function)(int64_t atime);
    char                        name[TASK_NAME_LEN];
    uint8_t                     is_print;
    struct periodic_task_desc * next;
};

int reg_task     (char * abr, struct periodic_task_desc * task_desc, int64_t shift_ns, int64_t period_ns, int64_t span_ns, int64_t atime, void function());
void set_to       (            struct periodic_task_desc * task_desc,                   int64_t delay_ns,                   int64_t atime);
void update_period(            struct periodic_task_desc * task_desc,                   int64_t period_ns);
void stop_task    (struct periodic_task_desc * task_desc, int64_t itime);
//void stop_all_task_to_update_fw(int64_t itime);


int64_t print_task_stat(struct periodic_task_desc * task_desc, int64_t itime);

int64_t main_body();
int64_t soft_timer(struct periodic_task_desc *sq, int64_t itime);
int8_t init_app_timer_ns(int64_t interval_ns);


#endif //TIMER_APP_H
