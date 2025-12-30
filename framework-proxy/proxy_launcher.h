
#ifndef __PROXY_LAUNCHER_H__
#define __PROXY_LAUNCHER_H__


#include "npplugin_js_data.h"
#include "app_launcher.h"
#include "proxy_jni_function.h"


class ProxyLauncher : public AppLauncher
{
public:
    ProxyLauncher(){};

protected:

    const char* getClassNameMain();
    const char* getClassNameCommand();

    bool checkRunning(const int argc, const char** argv, const char* szAppParam);
    bool preAction(const int argc, const char** argv);
    bool postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv);
    bool processCommandFunction(cJSON* pRoot, const AppJniCommand* pJniCommand, const JNIEnv* pAppJniv);

private:
    int m_nPluginID;
    JsData* m_pJsData;
};

#endif
