/**
 * 该程序提供一些API，用于启动泰山Office
 * 

*/

#include <windows.h>
#include <stdio.h>
#include "fw_config.h"
#include "app_message.h"

#include "office_md.h"
/*
 *	函数功能：申请指定大小的内存空间
 *  函数参数：size 要申请的内存空间的大小
 *  返回：指向申请到的内存空间的指针
 */
void *MemAlloc(size_t size)
{
    void *p = malloc(size);
    if (p == 0) 
	{
		perror("malloc");
		exit(1);
    }
	
    return p;
}

/*
 *	函数功能：判断小敏菜单程序是否在运行
 *  函数参数：无
 *  返回：TRUE  小敏菜单程序正在运行
 *        FALSE 小敏菜单程序没有在运行
 */ 
BOOL IsRapidMenuExist()
{
	HANDLE hMutex = getMutex(RAPIDMENU);
	
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return TRUE;
	}

	CloseHandle(hMutex);
	return FALSE;
}

/*
 *  函数功能：将传入的虚拟机启动参数转变为虚拟机可识别的参数	
 *  函数参数：arg  传入的虚拟机启动参数
 *  返回：虚拟机可识别的参数
 *
 */
char* GetArguments(char* arg)
{
	if (strcmp(arg, "-verbosegc") == 0) 
	{
		return "-verbose:gc";		
	} 
	else if (strcmp(arg, "-t") == 0) 
	{
		return "-Xt";		
	} 
	else if (strcmp(arg, "-tm") == 0) 
	{
		return "-Xtm";		
	} 
	else if (strcmp(arg, "-debug") == 0) 
	{
		return "-Xdebug";		
	} 
	else if (strcmp(arg, "-noclassgc") == 0) 
	{
		return "-Xnoclassgc";		
	} 
	else if (strcmp(arg, "-Xfuture") == 0) 
	{
		return "-Xverify:all";		
	} 
	else if (strcmp(arg, "-verify") == 0) 
	{
		return "-Xverify:all";		
	} 
	else if (strcmp(arg, "-verifyremote") == 0) 
	{
		return "-Xverify:remote";		
	} 
	else if (strcmp(arg, "-noverify") == 0) 
	{
		//AddOption("-Xverify:none", NULL);
		return "-Xverify:none";
	} 
	else if (strncmp(arg, "-prof", 5) == 0) 
	{
		char *p = arg + 5;
		char *tmp = (char*)MemAlloc(strlen(arg) + 50);
		if (*p) 
		{
			sprintf(tmp, "-Xrunhprof:cpu=old,file=%s", p + 1);
		} 
		else 
		{
			sprintf(tmp, "-Xrunhprof:cpu=old,file=java.prof");
		}
		
		return tmp;	
	} 
	else if (strncmp(arg, "-ss", 3) == 0 ||
		strncmp(arg, "-oss", 4) == 0 ||
		strncmp(arg, "-ms", 3) == 0 ||
		strncmp(arg, "-mx", 3) == 0) 
	{
		char *tmp = (char*)MemAlloc(strlen(arg) + 6);
		sprintf(tmp, "-X%s", arg + 1); /* skip '-' */
		//AddOption(tmp, NULL);
		return tmp;
	}	
	else 
	{
		return arg;
	}
	
}

