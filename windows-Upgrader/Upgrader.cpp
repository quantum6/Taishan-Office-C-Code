#include <windows.h>
#include <TlHelp32.h>
#include <atlstr.h>
#include "UpgradeApi.h"
#include "UpgradeApiWindows.h"
#include "fw_upgrade.h"
#include "fw_log.h"
#include "fw_kit.h"
#include "app_message.h"

#define LOCK_NAME  "TSOfficeUpgrader"

#define RAPIDMENU_TRAYICON_ID 130
#define RAPIDMENU_EXE	L"RapidMenu.exe"

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hWnd, &dwPid);
    if(dwPid == lParam)
    {
        NOTIFYICONDATA tnd;
        tnd.cbSize = sizeof(NOTIFYICONDATA);
        tnd.hWnd   = hWnd;
        tnd.uID    = RAPIDMENU_TRAYICON_ID;
        ::Shell_NotifyIcon(NIM_DELETE, &tnd);
    }

    return TRUE;
}
 
void removeRapidMenuTrayIcon()
{
    DWORD pid = 0;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!Process32First(hSnapshot, &pe))
    {
		return ;
	}

    do
    {
        if(!Process32Next(hSnapshot, &pe))
            return ;
    } while (StrCmp(pe.szExeFile, RAPIDMENU_EXE));

    pid = pe.th32ProcessID;
    EnumWindows(EnumWindowCallBack, pid);
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
	const char* prodName = getProductName();
	if(strcmp(TS_OFFICE, prodName) == 0) 
	{
		removeRapidMenuTrayIcon();
		system("taskkill /f /im rapidmenu.exe");
	}

    int result = start_jni_app(CLASS_NAME_UPGRADER, LOCK_NAME, lpCmdLine);
    updateUninstallDisplayVersion();
    return result;
}