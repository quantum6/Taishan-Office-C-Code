
#ifndef __PRODUCT_LAUNCHER_H__
#define __PRODUCT_LAUNCHER_H__

#include "app_launcher.h"
#if defined(_WINDOWS)
#include <windows.h>
#endif

class ProductLauncher : public AppLauncher
{
public:
    ProductLauncher(){};

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
