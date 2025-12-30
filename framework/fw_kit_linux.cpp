#include <stdio.h>
#include <string.h>
#include <math.h>

#include <sys/stat.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <pwd.h>
#include <dirent.h>
#include <dlfcn.h>

#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"

#define TEMP_DIR "temp"

#define MAX_PING_TIME_SECOND	10

static char g_AppHome[BUFFER_SIZE] = {0};

char *strlwr(char *str)
{
    char* p = str;
    while((*p) != '\0')
    {
        if ((*p) >= 'A' && (*p) <= 'Z')
        {
            (*p) += 32;
        }
        p++;
    }
    return str;
}

static char* getRunningPath(const char* pExe, char* pPath)
{
    char command[BUFFER_SIZE] = {0};
    char exeDir[BUFFER_SIZE]  = {0};
    char linkName[BUFFER_SIZE]= {0};

    //TIO_LOG_TEXT(pExe);
    if (pExe[0] != '/')
    {
        // ./Office
		char* pName = strrchr((char*)pExe, '/');
		if (pName == NULL)
		{
		    pName = (char*)pExe;
		}
        else
        {
            pName += 1;
        }
        getcwd(exeDir, BUFFER_SIZE);
        sprintf(command, "%s/%s", exeDir, pName);
    }
    else
    {
        strcpy(command, pExe);
    }
    //TS_LOG_TEXT(command);

    int ret = readlink(command, linkName, BUFFER_SIZE-1);
    if (ret >= 0)
    {
        if (linkName[0] != '/')
        {
            sprintf(pPath, "%s/%s", exeDir, linkName);
        }
    }
    else
    {
        strcpy(pPath, command);
    }
	return pPath;
}

static void empty_dladdr()
{
    //
}

/**
 要考虑到多种情况。
 实际上得到的是bin
 */
char* os_initAppHome(const char* pExe)
{
    if (strlen(g_AppHome) > 0)
    {
        return g_AppHome;
    }

#if defined(__APPLE__)
	os_initMacAppHome(g_AppHome);
	return g_AppHome;
#endif

    //也可以取环境变量：TS_OFFICE_HOME
    if (pExe == NULL || strlen(pExe) == 0)
    {
        Dl_info dl_info;
        dladdr((void*)empty_dladdr, &dl_info);
        //kylin:/opt/Taishan/Office/Plugins/libnptsoffice.so
        //uos:  /opt/apps/cn.ts-it.office/entries/plugins/browser/libnptsoffice.so
        if (dl_info.dli_fname[0] != '/')
        {
            getcwd(g_AppHome, BUFFER_SIZE);
            strcat(g_AppHome, "/");
        }
        strcat(g_AppHome, dl_info.dli_fname);
		
		// UOS应用商店版本不使用/opt/Taishan目录
		char bin[16] = {0};
		sprintf(bin, "/%s", KEY_DIR_BIN);
        if (strstr(g_AppHome, TAISHAN_ROOT))
        {
            strchr(strstr(g_AppHome, TAISHAN_ROOT)+strlen(TAISHAN_ROOT)+1, '/')[0] = '\0';
        }
        else if (strstr(g_AppHome, APP_NAME_PREFIX_UOS))
        {
            strchr(strstr(g_AppHome, APP_NAME_PREFIX_UOS)+strlen(APP_NAME_PREFIX_UOS)+1, '/')[0] = '\0';
            sprintf(g_AppHome, "%s%c%s", g_AppHome, OS_DIR_CHAR, KEY_DIR_FILES);
        }
        else if(strstr(g_AppHome, bin))
        {
            strchr(strstr(g_AppHome, bin), '/')[0] = '\0';
        }

        return g_AppHome;
    }

    getRunningPath(pExe, g_AppHome);
    //like: /opt/Taishan/Office/bin/Office
    //remove Office
    strrchr(g_AppHome, '/')[0] = 0;
    //remove bin
    //Upgrader自身升级时，会从Office/Upgrader/sub目录下运行
    char* pInterPath = NULL;
    if (   (strlen(KEY_DIR_BIN)      > 0 && (pInterPath = strstr(g_AppHome, KEY_DIR_BIN     ))) != NULL
        || (strlen(KEY_DIR_UPGRADER) > 0 && (pInterPath = strstr(g_AppHome, KEY_DIR_UPGRADER))) != NULL
        || (strlen(KEY_DIR_PLUGINS)  > 0 && (pInterPath = strstr(g_AppHome, KEY_DIR_PLUGINS ))) != NULL
        )
    {
        *((char*)(pInterPath-1)) = 0;
    }
    return g_AppHome;
}

char* os_getAppHome(char* pPath)
{
    os_initAppHome(NULL);
    strcpy(pPath, g_AppHome);
    return pPath;
}


int  os_getLoginName(char* pName)
{
    struct passwd* pwd = getpwuid(getuid());
    strcpy(pName, pwd->pw_name);
    return 0;
}

int   os_getUserWorkHome(char* pName)
{
    char name[BUFFER_SIZE] = {0};
    os_getLoginName(name);
#ifdef __APPLE__
    sprintf(pName, "/Users/%s", name);
#else
    sprintf(pName, "/home/%s", name);
#endif
    return 0;
}

int   os_getProductWorkHome(char* pName)
{
    return os_getUserWorkHome(pName);
}

//如果进程退出（崩溃），锁自动消失。
bool os_isFileLocked(const char* pFileName)
{
    int fd = open(pFileName, O_WRONLY|O_CREAT, 0644);
    //文件打开失败，说明系统有问题，不需要运行了
    if (fd < 0)
    {
        TS_LOG_INFO("Open file %s error.", pFileName);
        return true;
    }

    //F_TLOCK：测试检测加锁情况
    if (lockf(fd, F_TLOCK, 0) < 0)
    {
        //printf(" lock error! \n");
        return true;
    }
	
	char* sudoUser = getenv("SUDO_USER");
	if(sudoUser != NULL && strlen(sudoUser) > 0)
	{
		char command[BUFFER_SIZE] = {0};
		sprintf(command, "chown %s %s", sudoUser, pFileName);
		system(command);
	}

    return false;
}

/**
 注意，要把进程放到后台运行，这样就不会阻塞。
 */
int os_executeProcess(const char* command, char* result)
{
    /*
	string strCmd = string(command);
	if(strCmd.length() > strlen(KEY_FILE_PRODUCT))
	{
		string sExeFileName = strCmd.substr(strCmd.length()-strlen(KEY_FILE_PRODUCT),strlen(KEY_FILE_PRODUCT));
		// 判断如果是要启动Office(小敏菜单相关的响应命令)，解决RapidMenu不能双击，没有响应的问题。
		if(strstr(command, KEY_FILE_PRODUCT) != NULL  && strstr(command, "?") != NULL )
		{
			char cmd[BUFFER_SIZE] = {0};
			sprintf(cmd,"%s &", command);
			system(cmd);
			return 0;
		}
	}
    */
	
    FILE *fstream = NULL;

    if (NULL == (fstream = popen(command, "r")))
    {
        return -1;
    }
	
    while (NULL != fgets(result, 128, fstream))
    {
        //TS_LOG_TEXT(result);
        break;
    }
    pclose(fstream);

    return 0;
}


/**
 注意system/popen都是阻塞的，运行不结束就不返回。
　1,所以这里加了后台运行符号　&
 2,为了简化外部操作，加了./
 */
int   os_executeProcessBackground(const char* psProcessName)
{
    char buff[1024] = {0};

    if (psProcessName == NULL || strlen(psProcessName) == 0)
    {
        return 0;
    }

    //TS_LOG_INFO("|||%s|||", psProcessName);
    //sprintf(buff, "./%s &", psProcessName);
    sprintf(buff, "%s &", psProcessName);
    system(buff);
    return 0;
}

void get_temp_path(char* pBuffer, const int bufLen)
{
	sprintf(pBuffer,"%s",TEMP_PATH);      
}
