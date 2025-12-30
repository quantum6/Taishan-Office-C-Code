
#ifndef __PRODUCT_LAUNCHER_H__
#define __PRODUCT_LAUNCHER_H__

#include "app_launcher.h"

class ProductLauncher : public AppLauncher
{
public:
    ProductLauncher(){};

protected:

#if defined(_WINDOWS)
	char*
#else
    key_t
#endif
		getMessageKey();

    const char* getClassNameMain();
    const char* getClassNameCommand();

   bool checkRunning(const int argc, const char** argv, const char* szAppParam);
   bool preAction(const int argc, const char** argv);
   bool postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv);
};

#endif
