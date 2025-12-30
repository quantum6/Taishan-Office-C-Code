#include <stdio.h>
#include <math.h>

#include <string.h>
#include <sys/stat.h>
#include <string>

#ifndef _WINDOWS

#ifndef __APPLE__
#include <glib.h>
#endif

#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#else
#include <io.h>
#include <windows.h>
#include <tchar.h>
#define F_OK    0
#endif

#include "fw_upgrade.h"
#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"

#ifdef __APPLE__
#include "../framework/inih/INIReader.h"
#endif

using namespace std;

#define SOFTWARE            "SOFTWARE"
#define WOW6432NODE         "WOW6432Node"
#define REG_KEY_ITEM_UNINSTALL_TAISHAN         "Microsoft\\Windows\\CurrentVersion\\Uninstall\\泰山"
#define REG_KEY_ITEM_DISPLAY_VERSION           "DisplayVersion"


#define SECTION_NAME      "config"
#define KEY_VERSION_CODE  "version_code"

#define OS_TEXT_KYLIN     "Kylin"
#define OS_TEXT_UOS       "UnionTech OS"


static void getOSName(char* pBuffer)
{
    FILE* pFile = fopen(OS_ISSUE_PATH, "r");
    if (pFile == NULL)
    {
        return;
    }
    
    char buf[BUFFER_SIZE] = {0};
    size_t nBytes = fread(buf, 1, BUFFER_SIZE, pFile);
    fclose(pFile);

    if (nBytes > 0)
    {
        if (strstr(buf, OS_TEXT_KYLIN) != NULL)
        {
            strcpy(pBuffer, OS_KYLIN);
        }
        else if (strstr(buf, OS_TEXT_UOS) != NULL)
        {
        	  strcpy(pBuffer, OS_UOS);
        }
        else
        {
        	  strcpy(pBuffer, OS_LINUX);
        }
    }
}

bool checkOSType(const char* osType)
{
    char osName[BUFFER_SIZE] = {0};
    getOSName(osName);
    if (strlen(osName) > 0 && strcmp (osName, osType) == 0)
    {
        return true;
    }
    return false;
}


bool isNotDigit(char ch)
{
    return !isdigit(ch);
}


char* getTempProductVersionFile(char* pBuffer)
{
    getTempWorkDir(pBuffer);
    sprintf(pBuffer, "%s/%s.%s", pBuffer, FILE_PRODUCT_VERSION_INFO, getProductName());
    return pBuffer;
}

/*
 升级包文件名结构(两种情形都有)：
 ts-office-version-OS-cpuArch.zip
 cn.ts-it.office-version-OS-cpuArch.zip

 version: 3.1.0.1154
*/
bool checkUpgradeFileVersion(char* pFileName)
{
    char currentVersion[BUFFER_SIZE] = {0};
    char upgradeVersion[BUFFER_SIZE] = {0};

    getInstalledVersion(currentVersion);
    if(strlen(currentVersion) == 0)
    {
        TS_LOG_INFO("Failed to get current installed version");
        return false;
    }

	char* temp = currentVersion;
	while (*temp != '\0')
	{
        if (*temp == '-')
		{
			*temp = '.';
		}
		temp ++;
	}

    temp = pFileName;
    temp = strchr(temp, '-')+1;
    temp = strchr(temp, '-')+1;
    strcpy(upgradeVersion, temp);
	temp = upgradeVersion;
	while (*temp != '\0')
	{
		char ch = *temp;
        if (ch == '-' || ch == '.'
			|| '0' <= ch && ch <= '9')
		{
		    temp ++;
			continue;
		}
		//back to -
		temp--;
		*temp = 0;
		break;
	}

	temp = upgradeVersion;
	while (*temp != '\0')
	{
        if (*temp == '-')
		{
			*temp = '.';
		}
		temp ++;
	}

    DEBUG_FORMAT("%s ? %s", upgradeVersion, currentVersion);
    if (strcmp(upgradeVersion, currentVersion) > 0)
	{
		return true;
	}
    return false;
}

int   getProductUpdatePackagePath(char* pPath)
{
    getProductWorkDir(pPath);
    strcat(pPath, OS_DIR_TEXT);
    strcat(pPath, KEY_DIR_UPGRADER);
    return 0;
}

bool  isUpdatePackageExist()
{
    char zipPath[BUFFER_SIZE] = {0};
    getProductUpdatePackagePath(zipPath);
    if(strlen(zipPath) == 0)
	{
        return false;
	}
    
    if (!searchExtFileInPath(zipPath, FILE_UPDATE_EXT))
    {
        if (!searchExtFileInPath(zipPath, FILE_UPGRADE_EXT))
        {
            return searchExtFileInPath(zipPath, FILE_UPGRADER_EXT);
        }
    }
    return true;
}

void getInstalledVersion(char* pBuffer)
{
    char iniFile[BUFFER_SIZE] = {0};
    getTempProductVersionFile(iniFile);
    if (!isPathExist(iniFile))
    {
         return;
    }

#ifdef _WINDOWS
    GetPrivateProfileStringA(
                            SECTION_NAME,       // section name
                            KEY_VERSION_CODE ,  // key name
                            "\0",               // default string
                            pBuffer,            // destination buffer
                            MAX_PATH,           // size of destination buffer
                            iniFile      // initialization file name
                        );
    //DeleteFileA(iniFile);
#else
    get_ini_string(iniFile, SECTION_NAME, KEY_VERSION_CODE, pBuffer);
#endif
    DEBUG_FORMAT("%s=%s", KEY_VERSION_CODE, pBuffer);
}

#ifdef _WINDOWS
static bool createProcess(LPSTR command, bool bHideWindow)
{
    PROCESS_INFORMATION  pi;
    STARTUPINFOA si;
        
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    if(bHideWindow)
    {       
        si.wShowWindow = SW_HIDE;
    }
        
    if( CreateProcessA( 
        NULL,                        
        command,                           
        NULL,                  
        NULL, 
        FALSE,
        bHideWindow ? CREATE_NO_WINDOW : 0,                           
        NULL,                         
        NULL,                         
        &si,
        &pi) != 0)
    {
        return true;
    }

    return false;
}

bool  searchExtFileInPath(const char* pPath, const char* pExt)
{   
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char DirSpec[MAX_PATH] = {0};
    strcpy(DirSpec, pPath);
	strcat(DirSpec, OS_DIR_TEXT);
    strcat(DirSpec, "*");
    strcat(DirSpec, pExt);
    hFind = FindFirstFileA(DirSpec, &findFileData);
    DWORD err = 0;
    if (hFind == INVALID_HANDLE_VALUE)
    {
        err = GetLastError();
        FindClose(hFind);
        return false;
    }

    bool found = false;
    do
    {
        if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
            && strcmp(findFileData.cFileName, "." ) == 0
            && strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }
        if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            continue;
        }
        if (strlen(findFileData.cFileName) > 0)
        {
            if(checkUpgradeFileVersion(findFileData.cFileName))
            {
                found = true;
                break;
            }
        }
    }while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return found;
}

/*
int   getUpdatePackagePath(char* pPath)
{
    HKEY hSubKey;
    DWORD dwSize = 1024;
    ::RegOpenKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
        0, KEY_READ, &hSubKey);
    ::RegQueryValueExA(hSubKey, "AppData", NULL, NULL, (LPBYTE)pPath, &dwSize);
    ::RegCloseKey(hSubKey);
    
    strcat(pPath, OS_DIR_TEXT);
    strcat(pPath, getProductDirName());
    strcat(pPath, OS_DIR_TEXT);
    strcat(pPath, KEY_DIR_UPGRADER);
    return 0;
}
*/

bool runAsAdmin(const char* strApp, UINT idd, const char* pCmdline)
{
	int bufLength = MAX_PATH + ((pCmdline == NULL) ? 0 : strlen(pCmdline));
    char* cmd = new char[bufLength];
	memset(cmd,0,bufLength);
	if(pCmdline)
		sprintf(cmd,"/adminoption %d %s",idd, pCmdline);
	else
		sprintf(cmd,"/adminoption %d",idd);

    SHELLEXECUTEINFOA execinfo;
    memset(&execinfo,0,sizeof(SHELLEXECUTEINFOA));
    execinfo.lpFile = strApp;
    execinfo.cbSize = sizeof(SHELLEXECUTEINFOA);
    execinfo.lpVerb = "runas";
    execinfo.fMask = SEE_MASK_NO_CONSOLE;
    execinfo.nShow = SW_SHOWNORMAL;
    execinfo.lpParameters = cmd;

    bool result = (bool)ShellExecuteExA(&execinfo);
	delete[] cmd;
	return result;
}

bool  startUpgrader(const char* exeName, const char* pCmdline)
{
    char szExePath[MAX_PATH * 15];
    os_getAppHome(szExePath);
    strcat(szExePath, OS_DIR_TEXT);
    strcat(szExePath, KEY_DIR_UPGRADER);
    strcat(szExePath, OS_DIR_TEXT);
    strcat(szExePath, exeName);
    strcat(szExePath, ".exe");
    if (strcmp(exeName,FILE_UPGRADER_EXE) == 0)
    {
        char tempFile[BUFFER_SIZE] = {0};
        getTempProductVersionFile(tempFile);
        //删除临时目录。
        remove(tempFile);
        
        return runAsAdmin(szExePath, 1, pCmdline);
    }
    return createProcess(szExePath, false); 
}

bool updateUninstallDisplayVersion()
{
    bool result = false;
    HKEY hKey = NULL;
	char regPath[MAX_PATH] = {0};
	getRegUninstallPath(regPath);
    LSTATUS status = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if(status == ERROR_SUCCESS)
    {
        char currentVersion[BUFFER_SIZE] = {0};
        getInstalledVersion(currentVersion);
        if(RegSetValueExA(hKey, REG_KEY_ITEM_DISPLAY_VERSION, NULL, REG_SZ, (const BYTE*)currentVersion, strlen(currentVersion)) == ERROR_SUCCESS)
        {   
            result = true;  
        }
        else
        {
            TS_LOG_INFO("Failed to update display version");
        }
    }
    else
    {
        TS_LOG_INFO("Failed to open uninstall registry");
    }

    if(hKey != NULL)    
        RegCloseKey(hKey);

    return result;  
}

bool isWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
    GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
			return false;
        }
    }
    return bIsWow64;
}

bool is64bitSystem()
{
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
        return true;
    else
        return false;
}

void getRegUninstallPath(char *result)
{
	sprintf(result,"%s\\",SOFTWARE);
	if((!is64bitSystem()) || isWow64())
	{
		strcat(result, WOW6432NODE);
		strcat(result,"\\");
	}
	strcat(result, REG_KEY_ITEM_UNINSTALL_TAISHAN);
	strcat(result, getProductDirName());
}
#else
bool  searchExtFileInPath(const char* pPath, const char* pExt)
{   
    bool bfound = false;
    struct dirent* filename = NULL;
    DIR* dir = opendir(pPath);
    if(dir == NULL)
        return bfound;
    while((filename = readdir(dir))!=NULL)
    {
        if(filename->d_type == DT_DIR)
            continue;
        if(filename->d_type == DT_REG && string(filename->d_name).find(pExt) != string::npos)
        {
            string sFile = string(filename->d_name);
            string sExt = sFile.substr(sFile.length()-strlen(pExt),strlen(pExt));
            if(sExt != pExt)
                continue;
            
            if(checkUpgradeFileVersion(filename->d_name))
            {
                bfound = true;
                break;
            }
        }
    }
    closedir(dir);
    
    return bfound;
}

void get_ini_string(const char* iniFilePath, const char* sectionName, const char* keyName, char* value)
{
#ifndef __APPLE__
    GKeyFile* keyfile = g_key_file_new();
    g_key_file_load_from_file(keyfile,iniFilePath,G_KEY_FILE_NONE,NULL);
    gchar* strValue = g_key_file_get_string(keyfile,sectionName,keyName,NULL);
    if (strValue != NULL)
    {
        sprintf(value, "%s", strValue);
    }
#else
    INIReader reader(iniFilePath);
    if (reader.ParseError() < 0) {
        return ;
    }
    sprintf(value,"%s",reader.Get(sectionName, keyName, "").c_str());
#endif
}

bool  startUpgrader(const char* exeName, const char* pCmdline)
{
    char szExePath[BUFFER_SIZE];
    os_getAppHome(szExePath);
#ifdef __APPLE__
    if(strcmp(exeName,FILE_DOWNLOADER_EXE) == 0)
    {
        strcat(szExePath, OS_DIR_TEXT);
        strcat(szExePath, KEY_DIR_UPGRADER);
    }
    else
    {
        char tmp[32] = {0};
        sprintf(tmp, "/%s", getProductDirName());
        strstr(szExePath, tmp)[0] = 0;
        strcat(szExePath, "/TaishanOfficeUpgrader.app/Contents/MacOS");
    }
#else
    strcat(szExePath, OS_DIR_TEXT);
    strcat(szExePath, KEY_DIR_UPGRADER);
#endif
    strcat(szExePath,OS_DIR_TEXT);
    strcat(szExePath,exeName);
    
    pid_t pid = fork();
    if(pid == 0)      // child process
    {
        if(strcmp(exeName,FILE_DOWNLOADER_EXE) != 0)
        {
            int count = 0;
            const int MAX_SLEEP_SEC = 10;
            // 等待父进程退出
            while(getppid() != 1)
            {
                ++count;
                sleep(1);
                if(count >= MAX_SLEEP_SEC)
                    break;
            }
        }

         // 2次fork，避免产生僵尸进程，测试kylin amd64设置SIG_IGN忽略SIGCHLD信号无效
         if ((pid = fork()) < 0)
               _exit(-1); 
         else if (pid > 0)
               _exit(0);

        char command[BUFFER_SIZE] = {0};
        if(strcmp(exeName,FILE_DOWNLOADER_EXE) == 0)
        {
            sprintf(command,"%s",szExePath);
        }
        else
        {
            char tempDir[BUFFER_SIZE] = {0};
            getTempProductVersionFile(tempDir);
            //删除临时目录。
            remove(tempDir);
            
            char terminalTitle[BUFFER_SIZE] = {0};
            sprintf(terminalTitle, "泰山%s软件升级", getProductName());
#ifdef __APPLE__
            sprintf(command,"%s", szExePath);
#else
            if (checkOSType(OS_UOS))
            {
                sprintf(command, "%s -e 'sudo -E %s'", TERMINAL_UOS, szExePath);
            }
            else
            {
                sprintf(command, "%s -t '%s' -x sh -c 'sudo kill -9 `pgrep RapidMenu` ; sudo -E %s'", TERMINAL_MATE, terminalTitle, szExePath);
            }
#endif
        }

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(errno);
    }
    else if (pid > 0) // parent process
    {
        if(strcmp(exeName,FILE_DOWNLOADER_EXE) != 0)
            exit(0);
        
         pid_t pid2;
         int status = 0;
		 int waitTimesMax = 10;
		 int waitTimes = 0;
         while((pid2 = waitpid(0,&status,WNOHANG)) >= 0)
         {
		 	++waitTimes;
			if(pid2 > 0 || waitTimes >= waitTimesMax)
				break;
		 }
        return true; 
    }
        
    return false;
}

#endif
