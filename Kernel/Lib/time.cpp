#include <time.h>

struct tm mktime_tm(int sec, int min, int hour, int day, int month, int year)
{
    int yday = day - 1;

    if (month > 1)
        yday += 31;
    if (month > 2)
        yday += 28;
    if (month > 3)
        yday += 31;
    if (month > 4)
        yday += 30;
    if (month > 5)
        yday += 31;
    if (month > 6)
        yday += 30;
    if (month > 7)
        yday += 31;
    if (month > 8)
        yday += 31;
    if (month > 9)
        yday += 30;
    if (month > 10)
        yday += 31;
    if (month > 11)
        yday += 30;

    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        yday += 1;

    tm t;
    t.tm_sec = sec - 1;
    t.tm_min = min - 1;
    t.tm_hour = hour - 1;
    t.tm_mday = day;
    t.tm_mon = month;
    t.tm_year = year - 1900;
    t.tm_wday = 0; // TODO
    t.tm_yday = yday;
    return t;
}

time_t mktime(struct tm *t)
{
    return t->tm_sec + t->tm_min * 60 + t->tm_hour * 3600 + t->tm_yday * 86400 +
           (t->tm_year - 70) * 31536000 + ((t->tm_year - 69) / 4) * 86400 -
           ((t->tm_year - 1) / 100) * 86400 + ((t->tm_year + 299) / 400) * 86400;
}
