#ifndef __JNI_APP_H__
#define __JNI_APP_H__

#include <jni.h>
#include <list>
#include "jni_runtime.h"
#include "fw_config.h"

class JniAppAbstract
{
public:

    void init()
    {
        bIsValid = false;
        pJniRuntime = NULL;
    }

    JniRuntime* getJniRuntime()
    {
        return pJniRuntime;
    }

    JNIEnv* getEnv()
    {
        return pJniRuntime->pEnv;
    }
    
    bool isValid()
    {
        return bIsValid;
    }

    virtual bool start() = 0;
    virtual void release()= 0;

    JniRuntime* pJniRuntime;
    bool bIsValid;
};

#endif

