#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <strsafe.h>

#include "fw_kit.h"
#include "fw_config.h"
#include "fw_log.h"

#pragma warning(disable:4995)

#define REG_KEY_VALUE_INSTALL_DIR			"install_dir"

static char g_AppHome[BUFFER_SIZE] = {0};

static char* readRegKeyInstallDir(char* pBuffer, const char* product)
{
	HKEY  hKeyResult = NULL;
	DWORD dwSize     = 0;
	DWORD dwDataType = 0;
	char reg_key[BUFFER_SIZE] = {0};
	sprintf(reg_key, "%s%s", REG_KEY_ITEM_TAISHAN, product);

	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, reg_key, 0, KEY_READ, &hKeyResult))
	{
		::RegQueryValueExA(hKeyResult, REG_KEY_VALUE_INSTALL_DIR, 0, &dwDataType, NULL,            &dwSize);
		::RegQueryValueExA(hKeyResult, REG_KEY_VALUE_INSTALL_DIR, 0, &dwDataType, (LPBYTE)pBuffer, &dwSize);
		//DEBUG_TEXT(pBuffer);
	}
	::RegCloseKey(hKeyResult);
	return pBuffer;
}


//windows下获取当前dll的句柄
static HMODULE GetSelfModuleHandle()
{
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}

static char* getRunningPath(const char* pExe, char* pBuffer, const UINT appPathSize)
{
	// 取得Office.exe的全路径，如"c:\progra~1\Taishan\Office\bin\Office.exe"
	if (GetModuleFileNameA(
		(pExe != NULL && strlen(pExe) > 0) ? NULL : GetSelfModuleHandle(),
		pBuffer, appPathSize) == 0)
	{
		return NULL;
	}
	return pBuffer;
}

char* os_initAppHome(const char* pExe)
{
	if (strlen(g_AppHome) > 0)
	{
		return g_AppHome;
	}

	// 先读取注册表
	/*
	readRegKeyInstallDir(g_AppHome, pExe);
	if (strlen(g_AppHome) > 0)
	{
		return g_AppHome;
	}
	*/

	//再获取当前运行路径。
	getRunningPath(pExe, g_AppHome, BUFFER_SIZE);
	if (strlen(g_AppHome) == 0)
	{
		return g_AppHome;
	}

	//remove Office
	strrchr(g_AppHome, OS_DIR_CHAR)[0] = 0;
	//remove bin
	if (   strstr(g_AppHome, KEY_DIR_BIN)      != NULL
		|| strstr(g_AppHome, KEY_DIR_UPGRADER) != NULL
		|| strstr(g_AppHome, KEY_DIR_PLUGINS)  != NULL)
	{
		strrchr(g_AppHome, OS_DIR_CHAR)[0] = 0;
	}
	return g_AppHome;
}


char* os_getAppHome(char* pPath)
{
	if (strlen(g_AppHome) <= 0)
    {
	    os_initAppHome(NULL);
    }

	strcpy(pPath, g_AppHome);
    return pPath;
}

int   os_getLoginName(char* pName)
{
	// 取得机器的用户名称，并以此名称来定义互斥量名称
	DWORD dwNameLen = MAX_NAME_SIZE;
	char szName[MAX_NAME_SIZE]         = {0};
	if (GetUserNameA(szName, &dwNameLen))
	{
		strcpy(pName, szName);
		return 0;
	}
	return -1;
}

int   os_getUserWorkHome(char* pName)
{
    unsigned int pathSize = GetEnvironmentVariableA("APPDATA", pName, BUFFER_SIZE);
	//DEBUG_TEXT(pName);
	return 0;
}

int   os_getProductWorkHome(char* pName)
{
    unsigned int pathSize = GetEnvironmentVariableA("ALLUSERSPROFILE", pName, BUFFER_SIZE);
	//DEBUG_TEXT(pName);
	return 0;
}

int os_executeProcess(const char* command, char* result)
{
    PROCESS_INFORMATION  pi;
    STARTUPINFOA si;
        
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    si.wShowWindow = SW_HIDE;
        
    if( CreateProcessA( 
        NULL,
        (LPSTR)command,
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi) != 0)
    {
        return 0;
    }

    return -1;
}

void os_changeWorkDirectory()
{
	wchar_t path[1024]={0};
	if (GetModuleFileNameW(NULL, path, sizeof(path)/sizeof(*path)) == 0)
	{
		return ;
	}
	
	wcsrchr(path, L'\\')[0] = 0;
	SetCurrentDirectoryW(path);
}

/*
static bool  os_searchFileInPath(const char* pFile, const char* pPath)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	char DirSpec[MAX_PATH];
	strcpy(DirSpec, pPath);
	strcat(DirSpec, "\\*");

	hFind = FindFirstFileA(DirSpec, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		return false;
	}

	bool found = false;
	while (FindNextFileA(hFind, &FindFileData) != 0)
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
			&& strcmp(FindFileData.cFileName, "." ) == 0
			&& strcmp(FindFileData.cFileName, "..") == 0)
		{
			continue;
		}
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			char DirAdd[MAX_PATH];
			strcpy(DirAdd, pPath);
			strcat(DirAdd, "\\");
			strcat(DirAdd, FindFileData.cFileName);
			return searchFileInPath(pFile, DirAdd);
		}
		if (!strcmp(pFile, FindFileData.cFileName))
		{
			found = true;
		}
	}

	FindClose(hFind);
	return found;
}
*/


/*
 *	函数功能：由GetLastError()的返回的错误代码得到对应的字符串
 *  函数参数：functionName  产生错误的函数名
 *  返回：    出错字符串 注意要释放此字符串
 */
char * getErrorString(const char * functionName)
{
	LPVOID lpMsgBuf;
	char * displayBuf;
	DWORD  dw = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_IGNORE_INSERTS, 
				  NULL,
				  dw, 
				  MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				  (LPTSTR)&lpMsgBuf,
				  0, NULL);

	int lenOfBuffer = (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)functionName) + 40)*sizeof(TCHAR);
	displayBuf = (char *)LocalAlloc(LMEM_ZEROINIT, lenOfBuffer);
	memset(displayBuf, 0, lenOfBuffer);

	StringCchPrintf((LPTSTR)displayBuf, LocalSize(displayBuf)/sizeof(TCHAR),
					TEXT("%s failed with error %d: %s"), functionName, dw, lpMsgBuf);

	char * msg = (char *)malloc(strlen(displayBuf) + 1);
	memset(msg, 0, strlen(displayBuf) + 1);
	strcpy(msg, displayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(displayBuf);

	return msg;
}

void get_temp_path(char* pBuffer, const int bufLen)
{
    GetTempPathA(bufLen,pBuffer);
}
