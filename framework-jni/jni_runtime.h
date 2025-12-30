#ifndef __JNI_RUNTIME_H__
#define __JNI_RUNTIME_H__

#include <jni.h>
#include <list>
#include "fw_kit.h"

using namespace std;


void  getJarPathApp(    char* pPath);


/**
  一个进程只能有一个虚拟机，却可以有多个实例（插件、OFFICE）。
  所以：
  JniRuntime与AbstractRuntime分开。
  AbstractRuntime中保留一个指针，这样大家可以共用一个虚拟机。
 */
class JniRuntime
{
public:
    JniRuntime()
    {
        this->pJvm = NULL;
        this->pEnv = NULL;
        this->jClassString = NULL;
        this->jStringUTF8  = NULL;
        this->nRefCount    = 0;
        memset(jarFilePath, 0, BUFFER_SIZE+1);
    }

    void copyFrom(const JniRuntime* pSrc)
    {
        this->pJvm         = pSrc->pJvm;
        this->pEnv         = pSrc->pEnv;
        this->jClassString = pSrc->jClassString;
        this->jStringUTF8  = pSrc->jStringUTF8;
    }

    int increaseRef()
    {
        nRefCount++;
        return nRefCount;
    }

    int decreaseRef()
    {
        nRefCount --;
        return nRefCount;
    }
    
    virtual ~JniRuntime()
    {
        release();
    }

    void setJarFile(const char* pJarFile)
    {
        if(pJarFile)
            sprintf(jarFilePath, "%s",pJarFile);
    }

    virtual bool start();
    bool start(const char* pJrePath);
    void release();
    virtual void addJarsToParamter(char classpath[], const char* pJarPath);

    jstring get_jstring(const char* buffer);
    jclass load_java_class(const char* className);
    jclass find_main_class(const char* pMainClass, const char* pJarPath);
    jobjectArray create_jvm_args(const int argc, const char** argv);
    jobjectArray create_jvm_args(const int argc, const char** argv, const char* extend);

    jobjectArray create_jargs(list<char*>& argList);
    jobjectArray createJvmArgs(const char * cmdLine);
    bool run_java_class(const jclass jMainClass, jobjectArray jArgs);

    JavaVM   *pJvm;
    JNIEnv   *pEnv;

    jclass    jClassString;
    jstring   jStringUTF8;
    int       nRefCount;
    char      jarFilePath[BUFFER_SIZE+1];
    
protected:
    int getJarsPath(char* pBuffer, const char* pDir);
};

#endif

