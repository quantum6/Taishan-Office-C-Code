#include "UpgradeApiWindows.h"
#include "fw_kit.h"
#include "fw_log.h"
#include "fw_config.h"
#include "app_message.h"
#include "..\framework-windows\DataExchange.h"
#include "UpgradeApi.h"


int start_jni_app(const char* className, const char* lock_name, LPWSTR lpCmdLine)
{
    //调用unPackage.exe启动Upgrader.exe，删除unPackage所在的文件夹，删除失败。
    //失败的原因是,unPackage.exe启动Upgrader.exe，Upgrader.exe的工作目录为unPackage.exe
    //所在目录。Upgrader.exe是不能删除工作目录。所以删除此文件夹失败了。
    //将工作目录切换到本身所在目录就可以了。
    os_changeWorkDirectory();
    //::MessageBoxA(NULL, "text1", "title", MB_OK);
    // 创建互斥量，以此保证同一个用户下只能运行一个Office 
    HANDLE hMutex = getMutex(lock_name);
    if (GetLastError() == ERROR_ALREADY_EXISTS || hMutex == NULL)
        return 0;

    char exeDir[BUFFER_SIZE] = {0};
    getAppUpgraderDir(exeDir);

    char cmdLineUTF8[1024]={0};
    if (wcslen(lpCmdLine) != 0)
    {
        if (convertUnicodeToUTF8(lpCmdLine, cmdLineUTF8, sizeof(cmdLineUTF8)) == false) {
            return -1;
        }
    }

    return runUpgrader(className, exeDir, cmdLineUTF8);
}
