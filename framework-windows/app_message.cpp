#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_message.h"
#include "fw_kit.h"
#include <algorithm>

HANDLE openMailSlot(const char* szMailSlotName)
{
	HANDLE hCreateFile = CreateFileA(szMailSlotName,
									GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,     // normal file 
									NULL); 
		
	if (hCreateFile == INVALID_HANDLE_VALUE) 
	{ 							
		// 如果打开邮槽失败，则等待100毫秒后再试，共尝试20次，若20次还是无法打开邮槽，则
		// 继续向下运行
		int waitTime = 0;
		do
		{	
			Sleep(100);
			hCreateFile = CreateFileA(szMailSlotName,
									GENERIC_WRITE,					
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,     // normal file 
									NULL); 				
				
			waitTime +=100;
				
		}while(hCreateFile == INVALID_HANDLE_VALUE && waitTime <= 100*20);
			
		if(GetLastError() != 0)
		{		
			SetLastError(0);
		}			
	}

	return hCreateFile;
}

static void sendMessageText(const HANDLE hCreateFile, const wchar_t* data)
{
	DWORD dwLen = 0;
	WriteFile(hCreateFile, data, (DWORD)(wcslen(data)*2), &dwLen, NULL);	
}

void sendMessages(const char* szMailSlotName, const int argc, const wchar_t** wargv)
{
	HANDLE hCreateFile = openMailSlot(szMailSlotName);
	if (hCreateFile == NULL)
	{		
		return;
	}
		
	// 若已有主程序在运行，则向已经在运行的主进程发送一个消息，
	// 使已经在运行的主进程主窗口显示在最前面
	if(__argc == 1)
	{
		sendMessageText(hCreateFile, TS_COMMAND_START);
	}
	else
	{
		// 向已经运行的主进程发送参数
		for(int i=1; i<argc; i++)
		{			
			sendMessageText(hCreateFile, *(wargv+i));
		}
	}
	sendMessageText(hCreateFile, TS_COMMAND_END);

	CloseHandle(hCreateFile);

	SetLastError(0);
}

void sendMessage(const char* szMailSlotName, const wchar_t* wargv)
{
	wchar_t* args[2] = {NULL, (wchar_t*)wargv};
	sendMessages(szMailSlotName, 2, (const wchar_t**)args);
}

/*
 *	函数功能：得到CPU主频
 *  函数参数：无
 *  返回：主频
 */
double GetCPUFrequencyMHz()
{
	HKEY hKey = NULL;
	DWORD status;
	DWORD dwSize;
	DWORD dwFrequency;
	double dMHz = 0.0;

	// Open registry for CPU information
	status = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
						  ("Hardware\\Description\\System\\CentralProcessor\\0"), 
						  0, 
						  KEY_QUERY_VALUE, 
	                      &hKey ); 
	if ( status == ERROR_SUCCESS )
	{	
		 // Query ~MHz information from registry
		 dwSize = sizeof(dwFrequency);
		 status = RegQueryValueExA (hKey, 
	   							   ("~MHz"), 
								   NULL, 
								   NULL,
								   (LPBYTE)&dwFrequency,
								   &dwSize );

		 if ( status == ERROR_SUCCESS )
		 {
 			dMHz = dwFrequency;
		 }
	}
	// Couldn't get CPU freq. from registry,
	// let's try to calibrate it	
	if (hKey != NULL)
	{
		RegCloseKey (hKey);
	}
	
	return dMHz;
}

static void appendUserName(char* pBuffer)
{
	// 取得机器的用户名称，并以此名称来定义互斥量名称
	DWORD dwNameLen = MAX_NAME_SIZE;
	char szName[MAX_NAME_SIZE]         = {0};
	if (GetUserNameA(szName, &dwNameLen))
	{
		strcat(pBuffer, "-");
		strcat(pBuffer, szName);
		//为什么出错？
		//sprintf(pBuffer,    "%s-%s", pBuffer,    szName);
	}
}

char* getNameFromInstallPath(char* pBuffer)
{
	if(os_initAppHome(NULL) == NULL || pBuffer == NULL)
		return pBuffer;

	strcat(pBuffer, os_initAppHome(NULL));
	string strName = pBuffer;
	std::replace(strName.begin(),strName.end(),'\\','/');
	std::replace(strName.begin(),strName.end(),':','/'); 
	sprintf(pBuffer,"%s",strName.c_str());
	return pBuffer;
}

HANDLE getMutex(const char* pName)
{
	char szMutexName[MAX_NAME_SIZE]    = {0};
	sprintf(szMutexName,    "%s",     pName);
	appendUserName(szMutexName);

	return CreateMutexA(NULL, TRUE, getNameFromInstallPath(szMutexName));
}

void getMailSlotName(char* pBuffer, const char* szApplication)
{
	// 邮槽名称，从而实现支持多用户。能否利用这个作为互斥条件？
	sprintf(pBuffer, "%s\\%s-%s", MAIL_SLOT_PATH, MAIL_SLOT_NAME, szApplication);
	appendUserName(pBuffer);
	getNameFromInstallPath(pBuffer);
}

void checkDirectoryEnd(char* pDir)
{
	int lastPos = (int)strlen(pDir)-1;
	if (pDir[lastPos] == '\\')
	{
		pDir[lastPos] = 0;
	}
}
