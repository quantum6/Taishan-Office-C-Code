#include <list>

#include "fw_kit.h"
#include "fw_config.h"
#include "fw_log.h"

#include "app_message.h"

#include "UpgradeApi.h"

bool UpgraderRuntime::start()
{
    char jre[BUFFER_SIZE]  = {0};
    getAppJdkDir(jre);

    return JniRuntime::start((const char* )jre);
}

void UpgraderRuntime::addJarsToParamter(char classpath[], const char* pJarPath)
{
    sprintf(classpath, "-Djava.class.path=");
    getJarsPath(classpath, pJarPath);
}

//cmdLine使用UTF-8编码
int UpgraderRuntime::startUpgrader(char * cmdLine)
{
    if (start() == false)
	{
        return 0;
	}

    jclass jc = load_java_class(class_name);
    if (jc == NULL)
	{
        return 0;
	}

    //jobjectArray arr = create_jvm_args(2, (const char **)parameters);
    jobjectArray arr = createJvmArgs(cmdLine);
    run_java_class(jc, arr);
    return 0;
}

int runUpgrader(const char* className, const char* jarPath, const char* cmdLine)
{
    UpgraderRuntime ur;
    
    sprintf(ur.class_name,"%s", className);
    
    ur.setJarFile(jarPath);
    
    char params[BUFFER_SIZE * 10] = {0};
    //for old.
    if (cmdLine != NULL && strlen(cmdLine) != 0)
    {
        strcpy(params, cmdLine);
    }
    
    strcat(params, " type=");
    strcat(params, PARAM_DEFAULT);

    strcat(params, " product=");
    strcat(params, getProductName());
    
    TS_LOG_TEXT(params);
    return ur.startUpgrader(params);
}