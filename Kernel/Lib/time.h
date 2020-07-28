#pragma once
#include <types.h>

struct timeval
{
    time_t tv_sec;
    int tv_usec;
};

struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct tm mktime_tm(int sec, int min, int hour, int day, int month, int year);
time_t mktime(struct tm *timeptr);
