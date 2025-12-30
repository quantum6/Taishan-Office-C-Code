
#ifndef __PROXY_LAUNCHER_H__
#define __PROXY_LAUNCHER_H__

#include "app_launcher.h"
#include <windows.h>

class ProxyLauncher : public AppLauncher
{
public:
    ProxyLauncher(){};

protected:

    char*  getMessageKey();

    const char* getClassNameMain();
    const char* getClassNameCommand();

   bool checkRunning(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow,
					 const char* szAppParam);
   bool preAction(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow);
   bool postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv);
};

#endif
