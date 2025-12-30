#ifndef __UPGRADE_API_H__
#define __UPGRADE_API_H__

#include "jni_runtime.h"

#define DIR_SUB "sub"

#define CLASS_NAME_UPGRADER   "tso.upgrader.Upgrader"
#define CLASS_NAME_DOWNLOADER "tso.upgrader.Downloader"

#define PARAM_DEFAULT "default"


class UpgraderRuntime : public JniRuntime
{
public:
    bool start();
    virtual void addJarsToParamter(char classpath[], const char* pJarPath);
    int startUpgrader(char * cmdLine);

public:
    char class_name[64];
};

int runUpgrader(const char* className, const char* jarPath, const char* cmdLine);


#endif // __UPGRADE_API_H__