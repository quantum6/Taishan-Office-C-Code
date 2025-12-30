#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"



void  set_log_level(const TS_LOG_LEVEL level)
{
    g_eDefaultLogEvel = level;
}

int  hex_dump(const char* data, const int count, const char* pFile, const int nSize, const char* pFunc)
{
#define HEX_BUFFER_LENGTH 2048

    char buffer[HEX_BUFFER_LENGTH] = {0};
    sprintf(buffer, "%s(%d)%s() size=%d:", pFile, nSize, pFunc, nSize);

    int max_count=(HEX_BUFFER_LENGTH-8-(int)strlen(buffer))/6;
    if (max_count>count)
    {
        max_count=count;
    }

    int units=max_count/4;
    for (int i=0; i<units; i++)
    {
        sprintf(buffer, "%s0x%02X, 0x%02X, 0x%02X, 0x%02X, ", buffer,
                (data[i*4+0] & 0xFF), (data[i*4+1] & 0xFF), (data[i*4+2] & 0xFF), (data[i*4+3] & 0xFF));
    }

    int left = max_count % 4;
    for (int i=0; i<left; i++)
    {
        sprintf(buffer, "%s0x%02X, ", buffer, (data[units*4+i] & 0xFF));
    }

    TS_LOG_TEXT(buffer);
    return 0;
}

int   write_text_to_logfile(const char* text)
{
    char logName[256] = {0};
    getUserWorkDir(logName);
    sprintf(logName, "%s%ctaishan_%s.log", logName, OS_DIR_CHAR, getProductName());
    FILE* log_fp = fopen(logName, "a+");
    if(log_fp == NULL)
	{
		return -1;
	}

    char timetext[64]   = {0};
    timestampText(timetext);
    fwrite(timetext, 1, strlen(timetext), log_fp);

	fwrite(text, 1, strlen(text), log_fp);

	fclose(log_fp);
	return 0;
}