

/** 不是太明白，BMP要自己处理吗？*/
#include "SplashWindow.h"
#include <process.h>

#include "fw_kit.h"
#include "fw_log.h"

#include "office_md.h"
#include "app_message.h"
#include "fw_config.h"
#include "fw_upgrade.h"
#include "app_common.h"
#include "app_jni_command.h"
#include "app_launcher.h"
#include "..\framework-windows\DataExchange.h"

#include "product_launcher.h"
#include "product_config.h"


#define READ_BUFFER_SIZE        (4*1024)
#define EXIT_THREAD_DELAY       (3*1000)

static HANDLE    g_hMailSlot     = NULL;  // TIO邮槽,用于打开TIO文件时传递文件名参数
static HANDLE    g_hMutex        = NULL;

// 显示启动画面
static HANDLE    g_hSplashThread = NULL;

       HWND      g_hWelcomeWnd   = NULL;


// 邮槽名称，从而实现TSOffice支持多用户。能否利用这个作为互斥条件？
static char g_szMailSlotName[MAX_NAME_SIZE] = {0};


/*
 *    函数功能：清理参数数组内存
 *  函数参数：args 指向参数数组, index 参数数组长度
 *  返回：无
 */
static void releaseArgs(TCHAR* args[], const int index)
{
    for(int i = 0; i < index; i++)
    {
        delete [] args[i];
    }
}

static void addArgs(TCHAR* args[], const int index, const TCHAR* buffer, const int length)
{
    args[index] = new wchar_t[length];
    memset(args[index], 0, sizeof(TCHAR)*length);
    wcscpy(args[index], buffer);
}

static DWORD WINAPI system_exit_thread(LPVOID lpParam)
{
    Sleep(EXIT_THREAD_DELAY);
    exit(0);
}

/**
 * 从邮槽中读取数据。
 *
 *    函数功能：从邮槽读取信息
 *  函数参数：lpEnv 指向JNI环境的指针
 *  返回：无
 */
static int WINAPI wait_and_read_from_mailslot(AppJniCommand* g_pAppJniCommand)
{
    TCHAR buffer[READ_BUFFER_SIZE] = {0};
    DWORD ReadBytes = 0;
    
    //存储参数数组
    TCHAR* szArgs[64] = {0};
    //总参数标记位
    int index = 0;
    while(TRUE)
    {
        memset(buffer, 0, sizeof(TCHAR)*READ_BUFFER_SIZE);
        if (ReadFile(g_hMailSlot, buffer, READ_BUFFER_SIZE, &ReadBytes, NULL) == 0)
        {
            break;
        }
        if (ReadBytes == 0)
        {
            continue;
        }

        // 如果接收到退出消息，则关闭通信邮槽和互斥量
        //MessageBoxW(NULL,TEXT("读取文件"),TEXT("测试1"),MB_OK);
        if (   wcscmp(buffer, TS_COMMAND_CLOSE) == 0
            || wcscmp(buffer, TS_COMMAND_EXIT ) == 0)
        {
            //在测试中发现，有时会卡住不退出。所以使用线程强制退出。
            DWORD dwThreadId = 0;
            CreateThread(NULL, 0, system_exit_thread, NULL, 0, &dwThreadId);

            releaseArgs(szArgs, index);
            index = 0;

            addArgs(szArgs, index, buffer, ReadBytes+1);
            index++;

            g_pAppJniCommand->transfer_command_to_shellmethods(index, (const TCHAR**)szArgs);
            break;
        }
        //参数结束标记
        else if (wcscmp(buffer, TS_COMMAND_END) != 0)
        {
            addArgs(szArgs, index, buffer, ReadBytes+1);
            index++;
            continue;
        }
        else
        {
            //没参数。
            if (index == 0)
            {    
                continue;
            }
            g_pAppJniCommand->transfer_command_to_shellmethods(index, (const TCHAR**)szArgs);

            releaseArgs(szArgs, index);
            index = 0;
        }
    }

    return 0;
}

static DWORD WINAPI ControlSplashWindow(HANDLE hThread, AppJniCommand* g_pAppJniCommand)
{
    double dblCpuFreq = GetCPUFrequencyMHz();

    JNIEnv* pEnv = g_pAppJniCommand->pJniRuntime->pEnv;

    // 设置ShellMethods类中的CPU频率变量，供PG播放时使用
    if (dblCpuFreq > 0.1)
    {
        jfieldID fid = pEnv->GetStaticFieldID(g_pAppJniCommand->jCommandClass, "cpuFreq", "Ljava/lang/String;");
        if (fid != NULL)
        {
            char szCpuFreq[MAX_PATH];
            sprintf(szCpuFreq, "%f", dblCpuFreq);
            pEnv->SetStaticObjectField(g_pAppJniCommand->jCommandClass, fid, pEnv->NewString((const jchar*)szCpuFreq, (jsize)strlen(szCpuFreq)));
        }
    }

    jfieldID fid = pEnv->GetStaticFieldID(g_pAppJniCommand->jCommandClass, "g_hWelcomeWnd", "I");
    if (fid != NULL)
    {
        pEnv->SetStaticIntField(g_pAppJniCommand->jCommandClass, fid, (jint)g_hWelcomeWnd);    
    }
    /*
    fid = env->GetStaticFieldID(jClassShellMethods, "isMainFrameShow", "Z");
    if (fid != NULL)
    {
        int count = 0;
        // 处理启动画面，每隔一定时间检测一次主TIO主界面是否已显示的标记，超过
        // 指定次数则直接结束检测
        while(!env->GetStaticBooleanField(jClassShellMethods, fid) && count < 100)
        {
            Sleep(30);
            count++;
        }
    }
    */

    if(g_hWelcomeWnd != NULL)
    {
        //KillTimer(g_hWelcomeWnd, 100);
        
        if(IsWindow(GetParent(g_hWelcomeWnd)))
        {
            PostMessage(GetParent(g_hWelcomeWnd), WM_QUIT, 0, 0);
        }
        else if(IsWindow(g_hWelcomeWnd))
        {
            PostMessage(g_hWelcomeWnd, WM_QUIT, 0, 0);
        }

        CloseHandle(hThread);
        g_hWelcomeWnd = NULL;
    }

    return 0;
}

char* ProductLauncher::getMessageKey()
{
	return "";
}

const char* ProductLauncher::getClassNameMain()
{
	return CLASS_MAIN;
}

const char* ProductLauncher::getClassNameCommand()
{
	return CLASS_COMMAND;
}


bool ProductLauncher::checkRunning(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow,
					 const char* szAppParam)
{
    char appName[32] = {0};
    getAppNameFromParam(appName, szAppParam);
    getMailSlotName(g_szMailSlotName, appName);

    // 创建互斥量，以此保证同一个用户下只能运行一个TSOffice
    char mutexName[MAX_PATH] = {0};
    sprintf(mutexName, "TSIT_Mutex_%s", appName);
    g_hMutex = getMutex(mutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        sendMessages(g_szMailSlotName, __argc, (const wchar_t**)__wargv);
        return false;
    }
    if (g_hMutex == NULL)
    {
        return false;
    }
	//点击小敏菜单的退出，启动时带参数。这个时候后面的不要操作啦。
    if (__argc > 1 && (wcscmp(__wargv[1], TS_COMMAND_CLOSE) == 0 || wcscmp(__wargv[1], TS_COMMAND_EXIT) == 0))
    {
        return false;
    }
	return true;
}

bool ProductLauncher::preAction(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow)
{
    if (isUpdatePackageExist())
    {
		char* pCmdline = convertUnicodeToMultiBytes(lpCmdLine);
        // 启动更新程序后退出
        if (startUpgrader(FILE_UPGRADER_EXE,pCmdline))
        {
			if(pCmdline != NULL)
				delete[] pCmdline;
            exit(0);
        }
    }
    else
    {
        // 启动下载更新包程序，继续运行
        startUpgrader(FILE_DOWNLOADER_EXE);
    }

    DWORD dwThreadId = 0;
    // 这个判断靠谱吗？从RapidMenu中启动这个带参数咱知道，机器启动不显示，点击本执行文件需要显示，能区分出来？
    // 如果不是机器启动时就启动TSOffice,则显示启动画面
    if (!IsRapidMenuExist() && 
        !((__argc >= 2) && ((wcscmp(*(__wargv+1), TEXT("???")) == 0) || (wcscmp(*(__wargv+1), TEXT("?q?")) == 0) || (wcscmp(*(__wargv+1), TS_COMMAND_CLOSE) == 0))))
    {
        g_hSplashThread = CreateThread(
            NULL,
            0,
            ShowWelcomeWnd,
            hInstance,
            0,
            &dwThreadId);
    }

    //启动小敏菜单
    char szPath[MAX_NAME_SIZE] = {0};
    GetModuleFileNameA(NULL, szPath, MAX_NAME_SIZE);
    strrchr(szPath, '\\')[0] = 0;
    char rapidMenu[MAX_NAME_SIZE] = {0};
    sprintf(rapidMenu, "%s\\%s", szPath, RAPIDMENU_EXE);
    ShellExecuteA(NULL,
          "open", rapidMenu,
          NULL, NULL, SW_MAXIMIZE);

    /* 创建一个用于TIO通讯的邮槽，邮槽采用不可靠连接，
     * 此处还采用邮槽的原因是：
     * 1 网络间邮槽确实不可靠，因为它不保证数据能够正确到达
     *   但本地邮槽可靠性很高。
     * 2 邮槽采用单向通信，消耗资源不大
     */
    g_hMailSlot = CreateMailslotA(g_szMailSlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES) NULL);
    if (g_hMailSlot == INVALID_HANDLE_VALUE)
    {
        //如果不能产生，确实是严重影响。而这个可能性很小。
        SetLastError(0);
    }

	return false;
}

bool ProductLauncher::postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv)
{
    ControlSplashWindow(g_hSplashThread, (AppJniCommand*)pAppJni);
    // 用于减少启动时占用的物理内存，物理内存占用值降低10M左右。
    ::SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
    //这是个死循环，接收处理消息。由某个条件中断。
    wait_and_read_from_mailslot((AppJniCommand*)pAppJni);

	
    // 若通信邮槽没关闭，则关闭之。
    if(g_hMailSlot != NULL)
    {
        CloseHandle(g_hMailSlot);
        g_hMailSlot = NULL;
    }

    // 若互斥量没有关闭，则关闭之。
    if(g_hMutex != NULL)
    {
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
    }    

    ::FreeLibrary(NULL);
	return true;
}
