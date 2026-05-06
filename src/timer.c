#include <string.h>
#include "timer.h"
#include "app_time.h"
#include "global.h"
#include "systick.h"
#include "debug.h"


int debug_sleep = 0;

uint8_t debug_tasks = 0;


static const int64_t USEC_PER_SEC = 1000000ul;

struct periodic_task_desc * task_list_head = 0;

int64_t soft_timer(struct periodic_task_desc * sq, int64_t atime);

int64_t main_body()
{
    int64_t atime = get_atime();

    int64_t next_atime = 0x7fffffffffffffff;

    struct periodic_task_desc * task_desc;

    for(task_desc = task_list_head; task_desc != 0; task_desc = task_desc->next)
    {
        atime = get_atime();
        int64_t next_task_atime = soft_timer(task_desc, atime);
        if(next_task_atime && next_atime > next_task_atime) next_atime = next_task_atime;

        if(debug_sleep) if(atime > 5000000000) print_time_s_ns(next_task_atime);
        if(debug_sleep) if(atime > 5000000000) printf(" next_task_atime task %s\n", task_desc->name);
    }

    if(debug_sleep) if(atime > 5000000000) print_time_s_ns(next_atime);
    if(debug_sleep) if(atime > 5000000000) printf(" next_atime \n");

    return next_atime;
}



int64_t soft_timer(struct periodic_task_desc * task_desc, int64_t atime)
{
    int16_t rand_ms = 0;
/*
    print_time_s_ns(atime);
    printf(" %s() task_desc->start_ns: ", __func__);
    print_time_s_ns(task_desc->start_ns);
    printf(" start_ns %lld  atime %lld  \n", task_desc->start_ns, atime);
*/

    if(task_desc->start_ns == 0   ) return task_desc->start_ns;
    if(task_desc->start_ns > atime) return task_desc->start_ns;

    int64_t old_start_ns = task_desc->start_ns;

    if(task_desc->period_ns > 1000ll)
    {
        for(int i = 0; i < 100; ++i)
        {

//g            if(task_desc->base_ns > atime) break;
            task_desc->start_ns += task_desc->period_ns;
            if(task_desc->start_ns > atime) break;
/*
            print_time_s_ns(atime);
            printf(" %s() %s stepping base_ns %lld.%09llu start_ns %lld.%09llu period_ns %lld.%09llu span %lld.%09llu half_span_ms %u divider %u\n"
                , __func__
                , task_desc->name
                , task_desc->base_ns /1000000000LL
                , task_desc->base_ns %1000000000LL
                , task_desc->start_ns/1000000000LL
                , task_desc->start_ns%1000000000LL
                , task_desc->period_ns/1000000000LL
                , task_desc->period_ns%1000000000LL
                , task_desc->span_ns/1000000000LL
                , task_desc->span_ns%1000000000LL
                , task_desc->half_span_ms
                , task_desc->divider);
*/
        }
    }

//g    task_desc->start_ns = task_desc->base_ns;

    if(task_desc->span_ns)
    {
//        int16_t rand = get_rand();
//        uint16_t rand_ms = rand / task_desc->divider;
//        if(rand_ms < task_desc->half_span_ms)task_desc->start_ns -= (int64_t)rand_ms/2.0 * 1000000LL;
//        else                                     task_desc->start_ns += (int64_t)rand_ms/2.0 * 1000000LL;

        rand_ms = 0;//get_rand(task_desc->half_span_ms*2);
        task_desc->start_ns += (int64_t)rand_ms * 1000000LL;

/*
        #ifdef WCH_CONTROLLER
            print_time_s_ns(atime);
            printf(" %s() %s rand %5u, rand_ms %3u, base_ns %u.%09u, start_ns %u.%09u, period_ns %u.%09u, span %u.%09u, half_span_ms %u, divider %u\n"
                , __func__
                , task_desc->name
                , rand
                , rand_ms
                , (uint32_t)(task_desc->base_ns /1000000000LL)
                , (uint32_t)(task_desc->base_ns %1000000000LL)
                , (uint32_t)(task_desc->start_ns/1000000000LL)
                , (uint32_t)(task_desc->start_ns%1000000000LL)
                , (uint32_t)(task_desc->period_ns/1000000000LL)
                , (uint32_t)(task_desc->period_ns%1000000000LL)
                , (uint32_t)(task_desc->span_ns/1000000000LL)
                , (uint32_t)(task_desc->span_ns%1000000000LL)
                , (uint32_t)(task_desc->half_span_ms)
                , task_desc->divider);
        #else
            print_time_s_ns(atime);
            printf(" %s() %s rand %5u rand_ms %3u base_ns %lld.%09llu start_ns %lld.%09llu period_ns %lld.%09llu span %lld.%09llu half_span_ms %u divider %u\n"
                , __func__
                , task_desc->name
                , rand
                , rand_ms
                , task_desc->base_ns /1000000000LL
                , task_desc->base_ns %1000000000LL
                , task_desc->start_ns/1000000000LL
                , task_desc->start_ns%1000000000LL
                , task_desc->period_ns/1000000000LL
                , task_desc->period_ns%1000000000LL
                , task_desc->span_ns/1000000000LL
                , task_desc->span_ns%1000000000LL
                , task_desc->half_span_ms
                , task_desc->divider);
        #endif
*/
    }
    task_desc->function(atime);

    int64_t stop_atime = get_atime();

    int64_t exec_atime = stop_atime - atime;

    ++task_desc->count;
    task_desc->total_ns += exec_atime;
    if(task_desc->min_ns > exec_atime) task_desc->min_ns = exec_atime;
    if(task_desc->max_ns < exec_atime) task_desc->max_ns = exec_atime;
    if(task_desc->is_print)
    {

            print_time_s_ns(atime);
            printf(" %s() %s rand_ms %3u base_ns %lu.%09lu start_ns %lu.%09lu period_ns %lu.%09lu span %lu.%09lu half_span_ms %lu divider %u\n"
                , __func__
                , task_desc->name
                , rand_ms
                , (uint32_t)(task_desc->base_ns /1000000000LL)
                , (uint32_t)(task_desc->base_ns %1000000000LL)
                , (uint32_t)(task_desc->start_ns/1000000000LL)
                , (uint32_t)(task_desc->start_ns%1000000000LL)
                , (uint32_t)(task_desc->period_ns/1000000000LL)
                , (uint32_t)(task_desc->period_ns%1000000000LL)
                , (uint32_t)(task_desc->span_ns/1000000000LL)
                , (uint32_t)(task_desc->span_ns%1000000000LL)
                , (uint32_t)(task_desc->half_span_ms)
                , task_desc->divider);


    }
    return task_desc->start_ns;

}

int reg_task(char * abr, struct periodic_task_desc * task_desc, int64_t shift_ns, int64_t period_ns, int64_t span_ns, int64_t atime, void function())
{
    task_desc->is_print = 0;
    if(span_ns > period_ns)
    {
        print_time_s_ns(atime);
        printf(" %s() %s FATAL ERROR: period %lld.%09llu should be greater than span %lld.%09llu\n"
            , __func__
            , task_desc->name
            , task_desc->period_ns/1000000000LL
            , task_desc->period_ns%1000000000LL
            , task_desc->span_ns/1000000000LL
            , task_desc->span_ns%1000000000LL
        );
        return 1;
    }

    for(int i = 0; i < TASK_NAME_LEN; ++i)
        task_desc->name[i] = '\0';

    for(int i = 0; i < TASK_NAME_LEN - 1; ++i)
    {
        if(abr[i] == 0) break;
        task_desc->name[i] = abr[i];
    }

    task_desc->base_ns   = (atime/1000000000LL + 1)*1000000000LL + shift_ns;
    task_desc->start_ns  = task_desc->base_ns;

    task_desc->period_ns = period_ns;
    task_desc->function  = function;
    task_desc->span_ns   = span_ns;

    task_desc->total_ns = 0;
    task_desc->max_ns   = 0;
    task_desc->min_ns   = 1000000000LL;

    task_desc->count    = 0;

    task_desc->half_span_ms = (uint16_t)(task_desc->span_ns / 2000000LL);
    // if(span_ns > 0)
    // {
    //     task_desc->divider = (uint16_t)ceil(65536.0 / ((int64_t)task_desc->half_span_ms*2.0));
    // }

    if(debug_tasks)
    {
        print_time_s_ns(atime);
        printf(" %s() %s base_ns %u.%09lu start_ns %ld.%09lu period_ns %ld.%09lu span %d.%09lu half_span_ms %u divider %u\n"
            , __func__
            , task_desc->name
            , (uint32_t)(task_desc->base_ns   / 1000000000LL)
            , (uint32_t)(task_desc->base_ns   % 1000000000LL)
            , (uint32_t)(task_desc->start_ns  / 1000000000LL)
            , (uint32_t)(task_desc->start_ns  % 1000000000LL)
            , (uint32_t)(task_desc->period_ns / 1000000000LL)
            , (uint32_t)(task_desc->period_ns % 1000000000LL)
            , (uint32_t)(task_desc->span_ns   / 1000000000LL)
            , (uint32_t)(task_desc->span_ns   % 1000000000LL)
            , task_desc->half_span_ms
            , task_desc->divider);
    }

    task_desc->next = task_list_head;
    task_list_head = task_desc;
    return 0;
}

void set_to(struct periodic_task_desc * task_desc, int64_t delay_ns, int64_t atime)
{
    task_desc->start_ns  = atime + delay_ns;
    task_desc->period_ns = 0;
    task_desc->span_ns   = 0;
    task_desc->half_span_ms = 0;
    task_desc->divider   = 1;

//    printf("%s(%lld) ----------------------------------------------------------------------------------------\n"
//        , __func__, task_desc->start_ns/1000000000L);

    if(debug_tasks)
    {
        print_time_s_ns(atime);
        printf(" %s() start_ns %lld.%09llu\n"
            , __func__
            , task_desc->start_ns / 1000000000LL
            , task_desc->start_ns % 1000000000LL);
    }
}

void stop_task(struct periodic_task_desc * task_desc, int64_t itime)
{
    task_desc->start_ns  = 0;
    task_desc->period_ns = 0;
    task_desc->span_ns   = 0;
    task_desc->half_span_ms = 0;
    task_desc->divider   = 1;

    if(debug_tasks)
    {
        print_time_s_ns(itime);
        printf(" %s() start_ns %lld.%09llu\n"
            , __func__
            , task_desc->start_ns/1000000000LL
            , task_desc->start_ns%1000000000LL);
    }
}

void stop_task_by_name(char *name, int64_t itime)
{
    printf("%s()\n", __func__);
    struct periodic_task_desc * task_desc;
     for(task_desc = task_list_head; task_desc != 0; task_desc = task_desc->next)
    {
        itime = get_itime();
        if (!strcmp(task_desc->name, name)) 
        {
            stop_task(task_desc, itime);
            printf("stop task: %s\n", task_desc->name);
            return;
        }
    }
}


// void stop_all_task_to_update_fw(int64_t itime)
// {
//     printf("%s()\n", __func__);

//     struct periodic_task_desc * task_desc;

//     for(task_desc = task_list_head; task_desc != 0; task_desc = task_desc->next)
//     {
//         itime = get_itime();
//         if(mei_type == MBS_MEI_TYPE) if (!strcmp(task_desc->name, "RxExtMEI")) continue;
//         if(mei_type == MAM_MEI_TYPE) if (!strcmp(task_desc->name, "RxIntMEI")) continue;
//         if (!strcmp(task_desc->name, "RxExtMEI")) continue;
//         if (!strcmp(task_desc->name, "WDT")) continue;
//         printf("stop task: %s\n", task_desc->name);
//         stop_task(task_desc, itime);
//     }
// }

void update_period(struct periodic_task_desc *task_desc, int64_t period_ns)
{
    task_desc->period_ns = period_ns;
}

int8_t init_app_timer_ns(int64_t interval_ns)
{
    return 0;
}

int64_t print_task_stat(struct periodic_task_desc * task_desc, int64_t itime)
{
    int64_t cur_itime = get_itime();

    int64_t total = 0.;

    if(task_desc->count <= 0) return total;

    total = 100. * (int64_t)task_desc->total_ns / ((int64_t)cur_itime - (int64_t)itime);

    //print_time_s_ns(itime);

        printf("%s ", task_desc->name);
        uint32_print(" count ", task_desc->count, 0, " ");
        uint32_print("min ", task_desc->min_ns / 1000000000LL, 0, ".");
        uint32_print("", (uint32_t)(task_desc->min_ns % 1000000000), 9, "");
        uint32_print(" avg ", (task_desc->total_ns / task_desc->count) / 1000000000LL, 0, ".");
        uint32_print("", (uint32_t)((task_desc->total_ns/task_desc->count) % 1000000000), 9, "");
        uint32_print(" max ", task_desc->max_ns / 1000000000LL, 0, ".");
        uint32_print("", (uint32_t)(task_desc->max_ns % 1000000000), 9, "");
        uint32_print(" total ", task_desc->total_ns / 1000000000LL, 0, ".");
        uint32_print("", (uint32_t)(task_desc->total_ns % 1000000000), 9, "\n");
        //uint32_print(" cpu ", (uint32_t)(total / 1000000000.00), 0, ".");
        //printf("", (uint32_t)(((uint64_t)(total * 1000000000.00))  % 1000000000), 9, "\n");
        //printf("min task_desc->count %lu\n", (uint32_t)(task_desc->min_ns %1000000000LL));


        return total;
}

/*
void print_tasks_(int64_t atime)
{
    int64_t total = 0.;

    printf(" %s() task_list_head %p\n", __func__, task_list_head);

    for(struct periodic_task_desc * task_desc = task_list_head; task_desc != 0; task_desc = task_desc->next)
    {

        printf(" %s() task_desc %p\n", __func__, task_desc);


        total += print_task_stat(task_desc, atime);
    }

    printf(" %s() total cpu %12.8f %%\n", __func__, total);
}
*/
/*
if(0)
{
    for(struct periodic_task_desc * task_desc = task_list_head; task_desc != 0; task_desc = task_desc->next)
    {
        total += print_task_stat(task_desc, itime);
        random_main(0, 100000UL);
    }

    printf(" %s() total cpu %12.8f %%\n", __func__, total);
}
*/
