#ifndef __APP_JNI_COMMAND_H__
#define __APP_JNI_COMMAND_H__

#include "jni_app.h"
#include "jni_runtime.h"
#include "fw_config.h"

class AppJniCommand : public JniAppAbstract
{
public:

    AppJniCommand(JniRuntime* pJava)
    {
        init();

        this->jMainClass     = NULL;
        this->jMainObject    = NULL;
        this->jCommandClass  = NULL;
        this->jCommandObject = NULL;
        this->jOpenMethod    = NULL;
        this->jAppExitMethod = NULL;
        this->pJniRuntime    = pJava;
    }

    void setClasses(const char* pMain, const char* pCommand);

    bool start(const int argc, const char** argv, const char* szApplication, const char* pJrePath, const char* pJarPath);
    bool start(const int argc, const char** argv, const char* szApplication);

    bool start();
    void release();

    void openFiles(list<char*>& argList);
    
    jclass find_shell_methods();
    
    void initCommandObject(const char* method, const char* signature);

#ifdef _WINDOWS
    void transfer_command_to_shellmethods(const int count, const wchar_t** szArgs);
#else
    void  open_file_in_office(list<char*>& argList);
    jobjectArray create_jargs(list<char*>& argList);
#endif

    void    jstringToBuffer(jstring jresult, char** ppBuffer);

    jclass    jCommandClass;
    jobject   jCommandObject;

private:
    char pClassNameMain[BUFFER_SIZE];
    char pClassNameCommand[BUFFER_SIZE];

    jclass    jMainClass;
    jobject   jMainObject;

    jmethodID jOpenMethod;
    jmethodID jAppExitMethod;
};

#endif  //__APP_JNI_COMMAND_H__
