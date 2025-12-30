#ifndef __FW_LOG_H__
#define __FW_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

/*
#ifdef _UNIX
#include <sys/syscall.h>
#include <unistd.h>
#endif
*/

#include <errno.h>

//仅仅依赖于这个。
#include "fw_timestamp.h"

//这样显得更酷一些
#define ASSERT(...)                 assert(__VA_ARGS__)
//extern int errno;


#ifndef TSOFFICE_LOG_FILE

typedef enum
{
    TS_LOG_LEVEL_NOTHING,
    TS_LOG_LEVEL_SYSTEM,
    TS_LOG_LEVEL_ERROR,
    TS_LOG_LEVEL_WARNING,
    TS_LOG_LEVEL_INFO
} TS_LOG_LEVEL;

static TS_LOG_LEVEL g_eDefaultLogEvel   = TS_LOG_LEVEL_ERROR;

#define TSOFFICE_LOG_FILE "tsoffice.log"
#if defined(_WINDOWS) || defined(WIN32)
#define __FUNC__    __FUNCTION__
#define TSOFFICE_LOG_FILE_PATH "d:\\" TSOFFICE_LOG_FILE
#else
#define __FUNC__    __func__
#define TSOFFICE_LOG_FILE_PATH "/tmp/" TSOFFICE_LOG_FILE
#endif //defined(_WINDOWS) || defined(WIN32)

#define TS_LOG_PRINT                    printf
#define TS_LOG_SHORT(  FMT, ...)        TS_LOG_PRINT(FMT "\n", ##__VA_ARGS__)
#define TS_LOG_NEWLINE()                TS_LOG_PRINT("\n");

#define TS_LOG_INCLUDE_FILE_NAME 1


#if TS_LOG_INCLUDE_FILE_NAME
#define TS_LOG_HEAD(level)              if (level > TS_LOG_LEVEL_NOTHING && level <= g_eDefaultLogEvel){ \
                                                char timetext[64]; timestampText(timetext); \
                                                TS_LOG_PRINT("%s:%s<%s>(%d)"
#define TS_LOG_TAIL                     "\n", timetext, __FILE__, __FUNC__, __LINE__
#else
#define TS_LOG_HEAD(level)              if (level > TS_LOG_LEVEL_NOTHING && level <= g_eDefaultLogEvel){ \
                                                char timetext[64]; timestampText(timetext); \
                                                TS_LOG_PRINT("%s:<%s>(%d)"
#define TS_LOG_TAIL                     "\n", timetext, __FUNC__, __LINE__
#endif


#define TS_LOG_SYSTEM( FMT, ...)        TS_LOG_HEAD(TS_LOG_LEVEL_SYSTEM)  " " FMT  TS_LOG_TAIL, ##__VA_ARGS__ );}
#define TS_LOG_ERROR(  FMT, ...)        TS_LOG_HEAD(TS_LOG_LEVEL_ERROR)   " " FMT  TS_LOG_TAIL, ##__VA_ARGS__ );}
#define TS_LOG_WARNING(FMT, ...)        TS_LOG_HEAD(TS_LOG_LEVEL_WARNING) " " FMT  TS_LOG_TAIL, ##__VA_ARGS__ );}
#define TS_LOG_TRACE(  FMT, ...)        TS_LOG_HEAD(TS_LOG_LEVEL_INFO)    " " FMT  TS_LOG_TAIL, ##__VA_ARGS__ );}

#define TS_LOG_HERE()                   TS_LOG_HEAD(g_eDefaultLogEvel)          TS_LOG_TAIL                );}
#define TS_LOG_TEXT(TEXT)               TS_LOG_HEAD(g_eDefaultLogEvel) " " "%s" TS_LOG_TAIL, TEXT          );}
#define TS_LOG_INT(INT)                 TS_LOG_HEAD(g_eDefaultLogEvel) " " "%d" TS_LOG_TAIL, INT           );}
#define TS_LOG_INFO(  FMT, ...)         TS_LOG_HEAD(g_eDefaultLogEvel) " " FMT  TS_LOG_TAIL, ##__VA_ARGS__ );}


#define TS_LOG_HEX_DUMP(buffer,size)    hex_dump(buffer, size, __FILE__, __LINE__, __FUNC__)

//使用毫秒.微秒的形式．这样看得更多一些．
#define COST_TIME_START()                long long ts_cost_time_start   = timestampMicroSeconds();
#define COST_TIME_HERE2(gh_min_time)    {long long ts_cost_time_current = timestampMicroSeconds();\
                                                 int ts_cost_time_diff=(int)(ts_cost_time_current-ts_cost_time_start);\
                                                 if (ts_cost_time_diff >= ts_min_time)TS_LOG_INFO("cost time=%2.3f", (ts_cost_time_diff/1000.0f));\
                                                 ts_cost_time_start = ts_cost_time_current;}
#define COST_TIME_HERE()                COST_TIME_HERE2(0)
#define COST_TIME_GET()                 (int)(timestampMicroSeconds()-ts_cost_time_start);

#define TIME_COST_START()               COST_TIME_START()
#define TIME_COST_HERE2(min_time)       COST_TIME_HERE2(min_time)
#define TIME_COST_HERE()                COST_TIME_HERE2(0)
#define TIME_COST_HERE()                COST_TIME_HERE2(0)
#define TIME_COST_GET()                 COST_TIME_GET()

#define CALL_ERRNO()                    TS_LOG_INFO("%d=%s", errno, strerror(errno))            

#define LOG_TEXT_ONLY(FMT, ...) char log_text[4096] = {0}; \
                                sprintf(log_text, "%s-%d:" FMT "\n", __FUNC__, __LINE__, ##__VA_ARGS__); \
                                printf("%s", log_text); 

#define LOG_TO_OUTPUT(FMT, ...) {LOG_TEXT_ONLY(FMT, ##__VA_ARGS__)}

#define LOG_FORMAT(FMT, ...)    LOG_TO_OUTPUT(FMT, ##__VA_ARGS__)
#define LOG_TEXT(text)          LOG_TO_OUTPUT("%s", text)
#define LOG_FLOAT(value)        LOG_TO_OUTPUT("%f", value)
#define LOG_INT(value)          LOG_TO_OUTPUT("%d", value)
#define LOG_HERE()              LOG_TO_OUTPUT("");

#define LOG_TO_FILE(FMT, ...)   {LOG_TEXT_ONLY(FMT, ##__VA_ARGS__) \
                                {FILE* log_fp = fopen(TSOFFICE_LOG_FILE_PATH, "a+"); \
                                 fwrite(log_text, 1, strlen(log_text), log_fp); fclose(log_fp);}}

#define DEBUG_FORMAT(FMT, ...)  LOG_TO_FILE(FMT, ##__VA_ARGS__)
#define DEBUG_TEXT(text)        LOG_TO_FILE("%s", text)
#define DEBUG_FLOAT(value)      LOG_TO_FILE("%f", value)
#define DEBUG_INT(value)        LOG_TO_FILE("%d", value)
#define DEBUG_HERE()            LOG_TO_FILE("");

#define FILE_FORMAT             DEBUG_FORMAT
#define FILE_TEXT               DEBUG_TEXT
#define FILE_FLOAT              DEBUG_FLOAT
#define FILE_INT                DEBUG_INT
#define FILE_HERE               DEBUG_HERE

#endif
//TSOFFICE_LOG_FILE


#ifdef __cplusplus
extern "C" {
#endif

void  set_log_level(const TS_LOG_LEVEL level);

int   hex_dump(const char* data, const int count, const char* pFile, const int nSize, const char* pFunc);

int   write_text_to_logfile(const char* text);

#ifdef __cplusplus
}
#endif

#endif //__FW_LOG_H__

