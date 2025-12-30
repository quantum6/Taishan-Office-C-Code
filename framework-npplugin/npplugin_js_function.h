
#ifndef __NPPLUGIN_JS_FUNCTION_H__
#define __NPPLUGIN_JS_FUNCTION_H__

#include "npapi.h"
#include "npfunctions.h"
#include "npruntime.h"
#include "npplugin_sdk.h"
#include "npplugin_js_data.h"


class JsCommander
{
public:
    JsCommander(const int pluginID);
    ~JsCommander();

    void sendCommandExit();
    void sendCommandFunction(char* function, const NPVariant* args, const uint32_t argCount);

    bool receiveResult(InstanceData* instance, NPVariant* result);

    int getControlShmID()
    {
        return m_pJsData->getControlShmID();
    }

private:
    JsData* m_pJsData;
};

/**
这两个函数直接实现即可。
 */
bool  JS_FUNCTION_init( NPVariant* result);
bool  JS_FUNCTION_getPluginVersion(NPVariant* result);

#endif // __NPPLUGIN_JS_FUNCTION_H__
