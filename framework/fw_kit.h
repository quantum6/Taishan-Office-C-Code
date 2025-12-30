#ifndef __FW_KIT_H__
#define __FW_KIT_H__

#ifdef _WINDOWS
#include <windows.h>
#include <string.h>
#else
#include <unistd.h>
#endif

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include <list>
using namespace std;

#define BUFFER_SIZE     512
#define MAX_NAME_SIZE   512

//if has os prefix, will implement at linux/window
char* os_initAppHome(const char* pExe);
char* os_getAppHome(char* pPath);

bool  os_isFileLocked(const char* pFileName);

int   os_executeProcess(const char* psProcessName, char* result);
void  os_changeWorkDirectory();

int   os_getLoginName(char* pName);
//windows we get C:\ProgramData, not userhome
int   os_getUserWorkHome(char* pName);
int   os_getProductWorkHome(char* pName);


/**
windows:  Taishan-Office
linux  : .Taishan-Office
 */
const char* getWorkDirName();

/**
/tmp/.Taishan-Office
 */
char* getTempWorkDir(char* pBuffer);


//int   os_executeProcessBackground(const char* psProcessName);
//bool  os_searchFileInPath(const char* pFile, const char* pPath);

/**
install path like:
/opt/apps/cn.ts-it.office : cn.ts-it.office
/opt/Taishan/Office       : Office
*/
const char* getProductDirName();

/**
ts-office
ts-reader
cn.ts-it.office
cn.ts-it.reader
*/
const char* getAppName();

/**
Office
Reader
*/
const char* getProductName();


char* getAppJdkDir(     char* pPath);
char* getAppBinDir(     char* pPath);
char* getAppLibDir(     char* pPath);
char* getAppConfigDir(  char* pPath);
char* getAppImagesDir(  char* pPath);
char* getAppPluginsDir( char* pPath);
char* getAppUpgraderDir(char* pPath);

char* addWorkDir(       char* pPath);
char* getUserWorkDir(   char* pPath);
char* getProductWorkDir(char* pPath);

char* concatDirFile(char* pPath, const char* pFile);

bool  isPathExist(const char* pPath);
void  get_temp_path(char* pBuffer, const int bufLen);

void  free_list_all(list<char*>& dataList);



//以下应该没用。
int   check_flag(int bit, int flags);

bool  is_file_exist(const char* pPath);

int   path_to_dirfile(const char* pPath, char* pDir, char* pFile);
int   get_file_size(const char* file_name);
int   make_dir_of_file(const char* pFileName);
int   make_dir(const char* pDirName);
int   make_dirs(char* pPath, int isLastFile);

char* ts_malloc(const int size);
char* get_login_name();

/*
 *	函数功能：由GetLastError()的返回的错误代码得到对应的字符串
 *  函数参数：functionName  产生错误的函数名
 *  返回：    出错字符串 注意要释放此字符串
 */
char* getErrorString(const char * functionName);

/* 这个写法错误 */
#if defined(__APPLE__)
void getAppMacOSPath(char* pPath);
void os_initMacAppHome(char* pPath);
#endif

/* windows有这个函数，linux没有。 */
#ifndef _WINDOWS
char *strlwr(char *str);
#endif

#endif //__FW_KIT_H__
