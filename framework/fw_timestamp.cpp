#include <stdio.h>
#include <time.h>

#ifdef _WINDOWS
#include <windows.h>
int gettimeofday(struct timeval* tp, void* tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year  = wtm.wYear  - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday  = wtm.wDay;
    tm.tm_hour  = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm.tm_isdst = -1;
    
    clock = mktime(&tm);
    tp->tv_sec  = (long)clock;
    tp->tv_usec = wtm.wMilliseconds*1000;

    return 0;
}
#else
#include <sys/time.h>
#endif

#include "fw_timestamp.h"


long timestampValue()
{
    struct tm *t;
    time_t tt;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time(&tt);
    t = localtime(&tt);
    return ((t->tm_hour*60+t->tm_min)*60+t->tm_sec);
}

long timestampMilliSeconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

long long timestampMicroSeconds()
{
    struct timeval tv;
    long long unit = 1000*1000;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec*unit + tv.tv_usec);
}

int timestampText(char* text)
{
    struct tm *t;
    time_t tt;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time(&tt);
    t = localtime(&tt);
    sprintf(text, "%02d-%02d %02d:%02d:%02d.%03d", 
         (int)(t->tm_mon+1),   (int)t->tm_mday,
         (int) t->tm_hour,     (int)t->tm_min, (int)t->tm_sec, (int)(tv.tv_usec/1000));
    return 0;
}
