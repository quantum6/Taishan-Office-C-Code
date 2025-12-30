#include <gtk/gtk.h>
#include <sys/msg.h>
#include <list>

#include "fw_log.h"
#include "fw_kit.h"

#include "fw_config.h"
#include "fw_resource.h"
#include "fw_config.h"
#include "fw_upgrade.h"
#include "app_launcher.h"
#include "app_common.h"
#include "app_message.h"


static AppJniCommand* g_pAppJniCommand = NULL;

#ifndef __APPLE__
#include <sys/prctl.h>
#else
#include <CoreServices/CoreServices.h>
static void dummyCallback(void * info) {};

struct JniRuntimePara
{
    int argc;
    const char** argv;
    const char* jarPath;
    const char* appParam;

    const char* jclassMain;
    const char* jclassCommand;
};

void* thread_func(void* pData)
{
    JniRuntimePara& jrtPara = *(JniRuntimePara*)pData;
    JniRuntime* pJniRuntime = new JniRuntime();
    pJniRuntime->setJarFile(jrtPara.jarPath);
    pJniRuntime->start();
    g_pAppJniCommand = new AppJniCommand(pJniRuntime);
    g_pAppJniCommand->setClasses(jrtPara.jclassMain, jrtPara.jclassCommand);

    if (!g_pAppJniCommand->start(jrtPara.argc, jrtPara.argv, jrtPara.appParam))
    {
        return NULL;
    }
}

#endif

int AppLauncher::launch(const int argc, const char** argv,
                     const char*     szAppParam,
                     const char*     szJarPath)
{
    if (!checkRunning(argc, argv, szAppParam) || preAction(argc, argv))
    {
        return 0;
    }

#ifndef __APPLE__
   
    //单独初始化，意思是可以有多个OFFIE实例。
    JniRuntime* pJniRuntime = new JniRuntime();
    pJniRuntime->setJarFile(szJarPath);
    pJniRuntime->start();
    g_pAppJniCommand = new AppJniCommand(pJniRuntime);

    g_pAppJniCommand->setClasses(getClassNameMain(), getClassNameCommand());
    if (!g_pAppJniCommand->start(argc, (const char**)argv, szAppParam))
    {
        return 0;
    }

    postAction(g_pAppJniCommand, pJniRuntime->pEnv);
#else
    /*
    // osx上如果显示splashwindow，java顶部菜单不会显示，原因尚不清楚
    show_action(NULL);
    signal(SIGALRM, exit_window);
    alarm(SHOW_WINDOW_TIME); */
    
    struct JniRuntimePara jrtPara;
    jrtPara.argc          = argc;
    jrtPara.argv          = (const char**)argv;
    jrtPara.jarPath       = szJarPath;
    jrtPara.appParam      = szAppParam;
    jrtPara.jclassMain    = getClassNameMain();
    jrtPara.jclassCommand = getClassNameCommand();
    
    CFRunLoopRef loopRef = CFRunLoopGetCurrent();
    pthread_t tid;
    pthread_create(&tid, NULL,  thread_func, &jrtPara);
    CFRunLoopSourceContext sourceContext = {
       .version = 0, .info = NULL, .retain = NULL, .release = NULL,
       .copyDescription = NULL, .equal = NULL, .hash = NULL,
       .schedule = NULL, .cancel = NULL, .perform = &dummyCallback};

    CFRunLoopSourceRef sourceRef = CFRunLoopSourceCreate(NULL, 0, &sourceContext);
    CFRunLoopAddSource(loopRef, sourceRef,  kCFRunLoopCommonModes);
    CFRunLoopRun();
    CFRelease(sourceRef);
#endif

    g_pAppJniCommand->release();
    delete g_pAppJniCommand;
    g_pAppJniCommand = NULL;
    return 0;
}
