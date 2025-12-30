
#ifndef __APP_LAUNCHER_H__
#define __APP_LAUNCHER_H__

#include "app_jni_command.h"

class AppLauncher
{
public:
    AppLauncher(){};

#if defined(_WINDOWS)
    int APIENTRY launch(
    const HINSTANCE hInstance,
    const HINSTANCE hPrevInstance,
    const LPWSTR     lpCmdLine,
    const int       nCmdShow,
#else
    int launch(
    const int argc, const char** argv,
#endif
                     const char*     szAppParam,
                     const char*     szJarPath);

protected:

    virtual const char* getClassNameMain() = 0;
    virtual const char* getClassNameCommand() = 0;

    virtual bool checkRunning(
#if defined(_WINDOWS)
    const HINSTANCE hInstance,
    const HINSTANCE hPrevInstance,
    const LPWSTR    lpCmdLine,
    const int       nCmdShow,
#else
		const int argc, const char** argv,
#endif
		const char* szAppParam) = 0;

    virtual bool preAction(
#if defined(_WINDOWS)
    const HINSTANCE hInstance,
    const HINSTANCE hPrevInstance,
    const LPWSTR    lpCmdLine,
    const int       nCmdShow
#else
		const int argc, const char** argv
#endif
		) = 0;

	virtual bool postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv) = 0;

};

#endif
