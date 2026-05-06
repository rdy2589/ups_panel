#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "app_time.h"
#include "systick.h"

int64_t cycle_start_atime   = 0;
int64_t app_start_mtime     = 0;
int64_t first_piv_msg_itime = 0;



int64_t itime_from_atime(int64_t atime)
{
    return atime - cycle_start_atime;
}


int64_t print_cur_itime_(void)
{
    int64_t itime = get_itime();
    print_time_s_ns(itime);
    return itime;
}

void print_time_s_ns(int64_t itime)
{
    int64_t print_itime = itime;

    printf("%lu.%09ld ", (uint32_t)(print_itime / 1000000000L), (uint32_t)(print_itime % 1000000000L));
   // print_itime_s_us(print_itime);

}

void print_time_ns_woffset(int64_t itime)
{
    int64_t print_itime = itime;

    printf("%5lu.%09ld ", (uint32_t)(print_itime / 1000000000L), (uint32_t)(print_itime % 1000000000L));
   // print_itime_s_us(print_itime);

}

void print_itime()
{
    int64_t time_ns = get_itime();
    print_time_s_ns(time_ns);
}

void print_time_s_us(int64_t itime)
{
    printf("%1lu.%06lu", (uint32_t)(itime / 1000000000L), (uint32_t)((itime % 1000000000L) / 1000));
}

int64_t rus_nanosleep(int64_t delay_ns)
{
    int64_t i;
    int64_t itime = get_itime();
    int64_t nitime;

    for(i = 0; ; ++i)
    {
        nitime = get_itime();

        if((nitime - itime) >= delay_ns) break;
    }
    return nitime;
}

// monotonic time
int64_t get_mtime()
{
    int64_t monotonic_ns = 0;

    monotonic_ns = get_sys_time_ns();

    return monotonic_ns;
}

// application time (since application started)
int64_t get_atime()
{
    return get_mtime() - app_start_mtime;
}

int64_t get_itime()
{
    int64_t atime = get_atime();
    return atime - cycle_start_atime;
}

void init_itime()
{
    cycle_start_atime = get_atime();
}

