
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

//注意，必须是这个位置。否则编译不过。
#include <tchar.h>

static AppJniCommand* g_pAppJniCommand   = NULL;

int APIENTRY AppLauncher::launch(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow,
                     const char*    szAppParam,
                     const char*     szJarPath)
{
    // 设置调用线程的区域为系统默认区域
    SetThreadLocale(GetUserDefaultLCID());

	if (!checkRunning(hInstance, hPrevInstance, lpCmdLine, nCmdShow, szAppParam) || preAction(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        return 0;
    }

    char tioJarPath[MAX_PATH] = {0}; // TIO的安装路径
    char jrePath[MAX_PATH]    = {0}; // jre路径变量
    //单独初始化，意思是可以有多个OFFIE实例。
    JniRuntime* pJniRuntime = new JniRuntime();
    pJniRuntime->setJarFile(szJarPath);
    pJniRuntime->start();
    g_pAppJniCommand = new AppJniCommand(pJniRuntime);
	g_pAppJniCommand->setClasses(getClassNameMain(), getClassNameCommand());

    if (!g_pAppJniCommand->start(__argc, (const char**)__wargv, szAppParam))
    {
        goto leave;
    }

	postAction(g_pAppJniCommand, pJniRuntime->pEnv);

leave:

    g_pAppJniCommand->release();
    delete g_pAppJniCommand;
    g_pAppJniCommand = NULL;

    pJniRuntime->release();
    delete pJniRuntime;
    pJniRuntime = NULL;

    //exit(0);
    return 0;
}
