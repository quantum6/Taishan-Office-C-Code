#ifndef __APPLE__
#include <sys/prctl.h>
#endif
#include <list>

#include "fw_kit.h"
#include "fw_config.h"
#include "fw_log.h"
#include "fw_upgrade.h"

#include "app_message.h"

#include "UpgradeApi.h"
#include "UpgradeApiLinux.h"

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>

static void dummyCallback(void * info) {};
extern void getAppMacOSPath(char* pPath);

CFRunLoopRef runLoop;

struct JniRuntimePara
{
    const char* className;
    const char* jarPath;
    const char* cmdLine;
};

void* thread_func(void* pData)
{
    JniRuntimePara* prt = (JniRuntimePara*)pData;
    runUpgrader(prt->className,prt->jarPath,prt->cmdLine);
    CFRunLoopStop(runLoop);
    exit(0);
}
#endif

int start_jni_app(const char* className, const int argc, const char** argv)
{
    os_initAppHome(argv[0]);

    char buffer[BUFFER_SIZE] = {0};
    if(strcmp(className, CLASS_NAME_DOWNLOADER) == 0)
    {
        get_lock_file(argv[0], FILE_DOWNLOADER_EXE, buffer);
    }
    else
    {
        // 忽略SIGHUP信号，避免升级结束Upgrader进程退出时，Office进程也跟着退出，不能正常启动Office
        signal(SIGHUP,SIG_IGN);
        char runPath[BUFFER_SIZE] = {0};
#ifndef __APPLE__
        /* 升级程序自身更新时，会从Office/Upgrader/sub目录下启动Upgrader，这里需要获得Upgrader所在的具体目录。
        *  getTioUpgraderDir返回的为固定的目录Office/Upgrader，所以这里不能使用getTioUpgraderDir。
        */
        if (readlink("/proc/self/exe", runPath, MAX_NAME_SIZE-1) <= 0)
        {
            TS_LOG_INFO("Failed to get Upgrader running path");
            return false;
        }
#else
        getAppMacOSPath(runPath);
#endif
        strrchr(runPath, '/')[0] = 0;

        // 判断是否是从Upgrader/sub目录启动
        if(strlen(runPath) > 0 && strstr(runPath, DIR_SUB) != NULL)
        {
            string sKey = DIR_SUB;
            sKey += ".";
            sKey += FILE_UPGRADER_EXE;
            get_lock_file(argv[0], sKey.c_str(), buffer);
        }
        else
        {
            get_lock_file(argv[0], FILE_UPGRADER_EXE, buffer);
        }

        //some linux need use jps find pid to kill        
        //system("killall Office");
        //system("killall RapidMenu");
    }
    
    //如果已经运行，退出
    if (os_isFileLocked(buffer))
    {
        TS_LOG_INFO("File is locked：%s", buffer);
        return 0;
    }

    char exeDir[BUFFER_SIZE] = {0};
#ifndef __APPLE__  
    memset(exeDir, BUFFER_SIZE, 0);
    if (readlink("/proc/self/exe", exeDir, MAX_NAME_SIZE-1) <= 0)
    {
        return false;
    }
#else
    getAppMacOSPath(exeDir);
#endif
    strrchr(exeDir, '/')[0] = 0;
    
#ifndef __APPLE__
	int res= runUpgrader(className, exeDir,
	    (argc > 1 && strlen(argv[1]) != 0) ? argv[1] : NULL);
	return res;
#else
    CFRunLoopRef loopRef = CFRunLoopGetCurrent();
    pthread_t tid;
    JniRuntimePara jrt;
    jrt.className = className;
    jrt.jarPath = exeDir;
    jrt.cmdLine = (argc > 1 && strlen(argv[1]) != 0) ? argv[1] : NULL;
    pthread_create(&tid, NULL,  thread_func, &jrt);
		
    CFRunLoopSourceContext sourceContext = {
       .version = 0, .info = NULL, .retain = NULL, .release = NULL,
       .copyDescription = NULL, .equal = NULL, .hash = NULL,
       .schedule = NULL, .cancel = NULL, .perform = &dummyCallback};

    runLoop = CFRunLoopGetCurrent();
    CFRunLoopSourceRef sourceRef = CFRunLoopSourceCreate(NULL, 0, &sourceContext);
    CFRunLoopAddSource(loopRef, sourceRef,  kCFRunLoopCommonModes);
    CFRunLoopRun();
		
    CFRelease(sourceRef);
    return 0;
#endif
}
