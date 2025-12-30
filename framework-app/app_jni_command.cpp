#include "fw_log.h"
#include "jni_runtime.h"
#include "app_jni_command.h"
#include "fw_config.h"


static jclass find_shell_methods(AppJniCommand* pJniCommand);

void AppJniCommand::setClasses(const char* pMain, const char* pCommand)
{
    memset(pClassNameMain, 0, BUFFER_SIZE);
    strcpy(pClassNameMain, pMain);

    memset(pClassNameCommand, 0, BUFFER_SIZE);
    strcpy(pClassNameCommand, pCommand);
}

bool AppJniCommand::start()
{
    return start(0, NULL, NULL);
}

bool  AppJniCommand::start(const int argc, const char** argv, const char* szApplication, const char* pJrePath, const char* pJarPath)
{
    if (this->pJniRuntime == NULL || !this->pJniRuntime->start(pJrePath))
    {
        return false;
    }

    this->pJniRuntime->increaseRef();
    JNIEnv* pEnv = this->pJniRuntime->pEnv;
    jclass jMainClass = this->pJniRuntime->find_main_class(pClassNameMain, pJarPath);
    if (jMainClass == NULL)
    {
        DEBUG_TEXT("mainClass=NULL");
        return false;
    }

    /*
    list<char*> argList;
    for (int i=1; i<argc; i++)
    {
        argList.push_back(strdup(argv[i]));
    }
    */
    jobjectArray args = this->pJniRuntime->create_jvm_args(argc, argv, szApplication);
    bool result = this->pJniRuntime->run_java_class(jMainClass, args);
    //free_list_all(argList);
    find_shell_methods();

    return result;
}

bool  AppJniCommand::start(const int argc, const char** argv, const char* szApplication)
{
    char jdk[BUFFER_SIZE]  = {0};
    getAppJdkDir(jdk);

    char jar[BUFFER_SIZE] = {0};
    getJarPathApp(jar);

    return start(argc, argv, szApplication, jdk, this->pJniRuntime->jarFilePath);

}

void AppJniCommand::release()
{
    // 通知java进程将要退出
    if(jAppExitMethod)
    {
        JNIEnv* pEnv = this->pJniRuntime->pEnv;
        pEnv->CallStaticObjectMethod(this->jCommandClass, this->jAppExitMethod, NULL);
    }

    // 结束子进程，如在线帮助界面
#ifndef _WINDOWS
    pid_t  pid = getpid();
    char buf[256] = {'\0'};
    sprintf(buf,"pgrep -P %d | xargs -r kill -9", pid);
    system(buf);
#endif
    
    if (this->jMainObject != NULL)
    {
        JNIEnv* pEnv = this->pJniRuntime->pEnv;
        jmethodID jmethod = pEnv->GetMethodID(this->jMainClass, "release", "()V");
        pEnv->CallVoidMethod(this->jMainObject, jmethod);

        this->pJniRuntime->decreaseRef();
        this->pJniRuntime->release();
    }
}

jclass AppJniCommand::find_shell_methods()
{
    if (this->jCommandClass == NULL)
    {
        JNIEnv* pEnv = this->pJniRuntime->pEnv;
        this->jCommandClass  = pEnv->FindClass(pClassNameCommand);
        this->jOpenMethod    = pEnv->GetStaticMethodID(this->jCommandClass, "onCommand", "([Ljava/lang/String;)V");
        this->jAppExitMethod = pEnv->GetStaticMethodID(this->jCommandClass, "onAppExit", "()V");
    }
    return this->jCommandClass;
}

void  AppJniCommand::openFiles(list<char*>& argList)
{
    if (argList.size() == 0
        || (this->jCommandClass == NULL && !find_shell_methods()))
    {
        return;
    }

    JNIEnv* pEnv = this->pJniRuntime->pEnv;
    jobjectArray args = this->pJniRuntime->create_jargs(argList);
    if (args == NULL)
    {
        return;
    }

    pEnv->CallStaticObjectMethod(this->jCommandClass, this->jOpenMethod, args);

    //释放内存？
}


#ifdef _WINDOWS
#include <windows.h>

jobjectArray create_main_args(const JniRuntime* pJniRuntime, const int count, const TCHAR** szArgs)
{

    //准备向主应用发送参数。
    jobjectArray args = pJniRuntime->pEnv->NewObjectArray(count, pJniRuntime->jClassString, NULL);
    if (args == NULL)
    {
        return args;
    }

    for (int j = 0; j < count; j ++)
    {
        //int max_len = wcslen(szArgs[j])*3;
        //char* lpsz = new char[max_len];
        //max_len = WideCharToMultiByte(CP_ACP, 0, szArgs[j], -1, lpsz, max_len, NULL, NULL);
        //jstring jstr=env->NewString((const jchar*)lpsz, max_len);
        jstring jstr=pJniRuntime->pEnv->NewString((const jchar*)(szArgs[j]), (jsize)(wcslen(szArgs[j])) );
        if (jstr != NULL)
        {
            pJniRuntime->pEnv->SetObjectArrayElement(args, j, jstr);            
        }
    }
    return args;
}

void AppJniCommand::transfer_command_to_shellmethods(const int count, const wchar_t** szArgs)
{
    jobjectArray args = create_main_args(this->pJniRuntime, count, (const TCHAR**)szArgs);
    this->pJniRuntime->pEnv->CallStaticObjectMethod(this->jCommandClass, this->jOpenMethod, args);        

    //args是否要释放？
}
#else

jobjectArray AppJniCommand::create_jargs(list<char*>& argList)
{
    if (argList.size() == 0)
    {
        return NULL;
    }

    jobjectArray args = this->pJniRuntime->pEnv->NewObjectArray(argList.size(),
        this->pJniRuntime->jClassString, NULL);
    if (args == NULL)
    {
        return NULL;
    }

    int i=0;
    for (list<char*>::const_iterator iter=argList.begin(); iter != argList.end(); iter++)
    {
        //TIO_LOG_INFO("%d, %s", i, *iter);
        jstring jstr = this->pJniRuntime->pEnv->NewStringUTF(*iter);
        if (jstr != NULL)
        {
            this->pJniRuntime->pEnv->SetObjectArrayElement(args, i, jstr);
            i++;
        }
    }

    return args;
}

void AppJniCommand::open_file_in_office(list<char*>& argList)
{
    if (argList.size() == 0 || !find_shell_methods())
    {
        return;
    }

    jobjectArray args = create_jargs(argList);
    if (args == NULL)
    {
        return;
    }

    this->pJniRuntime->pEnv->CallStaticObjectMethod(this->jCommandClass, this->jOpenMethod, args);

    //释放内存？
}

void AppJniCommand::initCommandObject(const char* method, const char* signature)
{
    JNIEnv* pEnv = this->pJniRuntime->pEnv;
    jmethodID getinstance = pEnv->GetStaticMethodID(     jCommandClass, method,      signature);
    jCommandObject        = pEnv->CallStaticObjectMethod(jCommandClass, getinstance, NULL);
}

void AppJniCommand::jstringToBuffer(jstring jresult, char** ppBuffer)
{
    JNIEnv* pEnv = this->pJniRuntime->pEnv;
    jmethodID  jmethod = pEnv->GetMethodID(this->pJniRuntime->jClassString, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray jarr    = (jbyteArray)pEnv->CallObjectMethod(jresult, jmethod, pEnv->NewStringUTF(CHARSET_UTF8));
    jsize      jlen    = pEnv->GetArrayLength(jarr);
    jbyte*     jdata   = pEnv->GetByteArrayElements(jarr, JNI_FALSE);

    if (*ppBuffer == NULL)
    {
        *ppBuffer = (char*)malloc((int)jlen+1);
        memset(*ppBuffer, 0, (int)jlen +1);
    }
    strncpy(*ppBuffer, (char*)jdata, (int)jlen);

    pEnv->ReleaseByteArrayElements(jarr, jdata, 0);
}

#endif
