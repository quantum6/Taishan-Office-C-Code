#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "fw_kit.h"

#define TAISHAN_OFFICE_KEY    L"Taishan\\Office"
#define UNINSTALL_EXE		  L"uninstall.exe"
#define PARAMETER_SILENT      L"--mode unattended"

static bool getInstallerName(wchar_t pBuffer[], int elementCount);
//createProcess不能启动带有管理员权限的exe程序 这里换成ShellExecuteEx方法。
static bool runAsAdmin(const wchar_t * strApp, HANDLE & handleOut, const wchar_t * cmdParameter);
static bool getInstallDirFromRegister(wchar_t pPath[], int elementCount, int & lengthOfPathOut);

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
    Sleep(1000);
    //MessageBoxA(NULL, "test1", "test", MB_OK);
    os_changeWorkDirectory();
    //执行安装程序。从当前目录找到exe文件。如果不是本身。则是安装包文件。
    wchar_t exeBuffer[1024];
    int count = sizeof(exeBuffer)/sizeof(*exeBuffer);
    memset(exeBuffer, 0, count);
    bool b = getInstallerName(exeBuffer, count-1);
    if (b == false) { 
        return -1;
    }

    wchar_t installDir[1024];
    memset(installDir, 0, sizeof(installDir));
    count = sizeof(installDir)/sizeof(*installDir);
	
    HANDLE handle;
    int lengthOfDir = 0;
    b = getInstallDirFromRegister(installDir, count, lengthOfDir);
    if (b) {
		wcscat(installDir, L"\\");
		wcscat(installDir, UNINSTALL_EXE);
		// 卸载当前安装版本，如果不先卸载，则无法进行安装
		if (runAsAdmin(installDir, handle, PARAMETER_SILENT) == false) {
			return -1;
		}

		// 等待卸载进程结束
		WaitForSingleObject(handle, INFINITE);
    }

	// 运行安装程序
    if (runAsAdmin(exeBuffer, handle, NULL) == false) {
        return -1;
    }

    //等待安装进程结束
    WaitForSingleObject(handle, INFINITE);
    
    //从注册表里读取安装包的安装目录
    memset(installDir, 0, sizeof(installDir));
    count = sizeof(installDir)/sizeof(*installDir);
    b = getInstallDirFromRegister(installDir, count, lengthOfDir);
    if (b == false) {
        return -2;
    }

    //拼接字符串
    wcscat(installDir, L"\\Upgrader\\upgrader.exe");
    wchar_t cmdParameter[128]={0};
    wcscat(cmdParameter, L"install;");
    wcscat(cmdParameter, exeBuffer);
    runAsAdmin(installDir, handle, cmdParameter);
    return 0;
}

static bool getInstallerName(wchar_t pBuffer[], int elementCount){
    WIN32_FIND_DATAW fileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    wchar_t * DirSpec=L"*.exe";

    hFind = FindFirstFileW(DirSpec, &fileData);
    if (hFind == INVALID_HANDLE_VALUE){
        FindClose(hFind);
        return false;
    }

    do {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            continue;
        }

        if (wcscmp(fileData.cFileName, L"Unpackager.exe") == 0) {
            continue ;
        }

        wcscpy(pBuffer, fileData.cFileName);
        //sprintf(pBuffer, "%s%s%s%s%s", pBuffer, pDir, OS_DIR_TEXT, fileData.cFileName, OS_SEP);
        return true;
    } while(FindNextFileW(hFind, &fileData) != 0);

    FindClose(hFind);
    return false;
}

//createProcess不能启动带有管理员权限的exe程序 这里换成ShellExecuteEx方法。
static bool runAsAdmin(const wchar_t * strApp, HANDLE & handleOut, const wchar_t * cmdParameter){

    //HANDLE hProcess;            // out, valid when SEE_MASK_NOCLOSEPROCESS specified
    SHELLEXECUTEINFOW execinfo;
    memset(&execinfo,0,sizeof(SHELLEXECUTEINFOW));
    execinfo.lpFile = strApp;
    execinfo.cbSize = sizeof(SHELLEXECUTEINFOW);
    execinfo.lpVerb = L"runas";
    execinfo.fMask = SEE_MASK_NO_CONSOLE | SEE_MASK_NOCLOSEPROCESS;
    execinfo.nShow = SW_SHOWNORMAL;
    execinfo.lpParameters = cmdParameter;

    bool b = ShellExecuteExW(&execinfo);
    handleOut = execinfo.hProcess;
    return b;
}

//函数功能:获得泰山Office的安装目录 目录编码为UTF16
//参数说明:
//pPath           字符数组 保存获得的安装目录字符串
//elementCount    字符数组元素个数
//lengthOfPathOut 获得的安装目录字符串长度 包括结尾的0
static bool getInstallDirFromRegister(wchar_t pPath[], int elementCount, int & lengthOfPathOut)
{
    HKEY hKey;
    DWORD dwSize = elementCount;
    //\\install_dir
    LSTATUS ret = RegOpenKeyExW(HKEY_CLASSES_ROOT, TAISHAN_OFFICE_KEY, 0, KEY_READ, &hKey);
    if (ret != ERROR_SUCCESS) {
        return false;
    }
    
    ret = RegQueryValueExW(hKey, L"install_dir", NULL, NULL, (LPBYTE)pPath, &dwSize);
    if (ret != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}
