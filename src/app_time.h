#ifndef _APP_TIME_H_
#define _APP_TIME_H_

//#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h>



void init_app_time();
void init_itime();

int64_t get_atime();
int64_t get_mtime();
int64_t get_itime();


void print_itime();
void print_time_us(int64_t itime);

int64_t itime_from_atime(int64_t atime);

void print_time_s_ns(int64_t itime);
int64_t print_cur_itime_();
int64_t rus_nanosleep(int64_t delay_ns);

#endif // APP_TIME_H
