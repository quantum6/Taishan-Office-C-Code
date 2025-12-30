#include <stdio.h>
#include <math.h>

#include <string.h>
#include <sys/stat.h>

#ifndef _WINDOWS
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <pwd.h>
#else
#include <io.h>
#define F_OK	0
#endif

#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"

#define TEMP_DIR "temp"

static char g_sProductDirName[BUFFER_SIZE] = {0};
static char        g_sAppName[BUFFER_SIZE] = {0};
static char    g_sProductName[BUFFER_SIZE] = {0};
static char    g_sWorkDirName[BUFFER_SIZE] = {0};

#define MAX_PING_TIME_SECOND    10

static char* getAppDir(char* pPath, const char* sub)
{
    os_getAppHome(pPath);
    if (strlen(sub) > 0)
    {
        sprintf(pPath, "%s%s%s", pPath, OS_DIR_TEXT, sub);
    }
    return pPath;
}

/**
 ProductName/AppName可以通过宏定义解决。
 考虑到通用性，还是直接在这里处理。
 !!! 注意不要使用DEBUG系列，会死循环。可以使用TS_LOG系列 !!!!
 将来要通过读取配置文件。
 1、ProductInfoTaishan每次初始化，都写一次product.ini
 2、从product.ini中读取。
 3、安装时限制命名。即用户选择目录，组合后必须以Taishan\Office(Reader)结尾。
 */
const char* getProductDirName()
{
    if (strlen(g_sProductDirName) == 0)
    {
        char* home = os_initAppHome(NULL);
        if (strstr(home, TAISHAN_ROOT))
        {
            char* product = strstr(home, TAISHAN_ROOT)+strlen(TAISHAN_ROOT)+1;
            strcpy(g_sProductDirName, product);
            strcpy(g_sProductName,    product);
            
            sprintf(g_sAppName, "%s%s", APP_NAME_PREFIX_KYLIN, product);
            strlwr(g_sAppName);
        }
        else if (strstr(home, APP_NAME_PREFIX_UOS))
        {
            char* product = strstr(home, APP_NAME_PREFIX_UOS);
            char* end = strchr(product, OS_DIR_CHAR);
            strncpy(g_sProductDirName, product, (end-product));

            product += strlen(APP_NAME_PREFIX_UOS);
            strncpy(g_sProductName,    product, (end-product));
            g_sProductName[0] -= 32;

            strcpy(g_sAppName,        g_sProductDirName);
        }
        else
        {
            strcpy(g_sProductDirName, home);
            strcpy(g_sProductName,    home);
            strcpy(g_sAppName,        home);
        }
    }

    //TS_LOG_TEXT(g_sProductName);
    return g_sProductDirName;
}

const char* getAppName()
{
    if (strlen(g_sAppName) == 0)
    {
        getProductDirName();
    }

    return g_sAppName;
}

const char* getProductName()
{
    if (strlen(g_sProductName) == 0)
    {
        getProductDirName();
    }

    return g_sProductName;
}

char* getAppJdkDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_JDK);
}

char* getAppBinDir(char* pPath)
{
    getAppDir(pPath, KEY_DIR_BIN);
    return pPath;
}

char* getAppLibDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_LIB);
}

char* getAppPluginsDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_PLUGINS);
}

char* getAppConfigDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_CONFIG);
}

char* getAppImagesDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_IMAGES);
}

char* getAppUpgraderDir(char* pPath)
{
    return getAppDir(pPath, KEY_DIR_UPGRADER);
}

const char* getWorkDirName()
{
    if (strlen(g_sWorkDirName) == 0)
    {
        getProductDirName();

/* LINUX 之前加个. */
#ifndef _WINDOWS
    strcat(g_sWorkDirName, ".");
#endif
    strcat(g_sWorkDirName, TAISHAN_ROOT);
    strcat(g_sWorkDirName, "-");
    strcat(g_sWorkDirName, g_sProductName);

    }
    return g_sWorkDirName;
}

char* getTempWorkDir(char* pBuffer)
{
    get_temp_path(pBuffer, BUFFER_SIZE);
    sprintf(pBuffer, "%s/%s", pBuffer, getWorkDirName());
    return pBuffer;
}

char* addWorkDir(char* pPath)
{
	strcat(pPath, OS_DIR_TEXT);

	strcat(pPath, getWorkDirName());
	return pPath;
}

char* getUserWorkDir(char* pPath)
{
    os_getUserWorkHome(pPath);
    addWorkDir(pPath);
    return pPath;
}

char* getProductWorkDir(char* pPath)
{
    os_getProductWorkHome(pPath);
    addWorkDir(pPath);
    return pPath;
}

char* concatDirFile(char* pDir, const char* pFile)
{
    sprintf(pDir, "%s%s%s", pDir, OS_DIR_TEXT, pFile);
    return pDir;
}

bool  isPathExist(const char* pPath)
{
    return (0 == access(pPath, F_OK));
}

int   writeDataToFile(const char* pFileName, const char* data_buffer, const int data_size)
{
    FILE* pFile        = fopen(pFileName, "wb");
    if (pFile == NULL)
    {
        return -1;
    }

    int writed         = 0;
    int left           = (int)  data_size;
    char* pWriteBuffer = (char*)data_buffer;
    while (left > 0)
    {
        writed        = (int)fwrite(pWriteBuffer, 1, left, pFile);
        //fflush(pFile);
        left         -= writed;
        pWriteBuffer += writed;
    }
    fflush(pFile);
    fclose(pFile);

    return 0;
}

int   check_flag(int bit, int flags)
{
    return bit & flags;
}

int   get_file_size(const char* file_name)
{
    struct stat statbuff;
    stat(file_name, &statbuff);
    return (int)(statbuff.st_size);
}

int   path_to_dirfile(const char* pPath, char* pDir, char* pFile)
{
    if (pPath == NULL)
    {
        return -1;
    }

    char* pCut = strrchr((char*)pPath, '/')+1;
    if (pCut == NULL)
    {
        strcpy(pFile, pPath);
        return 0;
    }

    //not need /(end)
    strncpy(pDir,  pPath, (pCut-pPath-1));
    strcpy( pFile, pCut);

    return 0;
}

void  free_list_all(list<char*>& dataList)
{
    for (list<char*>::const_iterator iter=dataList.begin(); iter != dataList.end(); iter++)
    {
        free(*iter);
    }
    dataList.clear();
}

char* ts_malloc(const int size)
{
    char* data = (char*)malloc(size);
    if (data != NULL)
    {
        memset(data, 0, size);
    }
    return data;
}

int make_dirs(char* pPath, int isLastFile)
{
    char buffer[BUFFER_SIZE] = {0};
    char* next = pPath;
    int isLast = 0;
    while (next != NULL)
    {
        next = strchr(next+1, '/');
        if (next == NULL)
        {
            if (isLastFile)
            {
                break;
            }
            isLast = 1;
            next = pPath+strlen(pPath);
        }
        strncpy(buffer, pPath, next-pPath);
        // DEBUG_TEXT(buffer);

        if ( access(buffer, F_OK) != 0
#ifndef _WINDOWS
           && mkdir(buffer, 0755) == -1
#endif
		   )
        {
            return -1;
        }

        if (isLast)
        {
            break;
        }
    }
    return 0;
}
