
#include "npplugin_js_listener.h"
#include "npplugin_app.h"

void npvariant_to_buffer(const NPVariant* var, char* buffer)
{
    NPString url = NPVARIANT_TO_STRING(*var);
    strncpy(buffer, url.UTF8Characters, url.UTF8Length);
}

static void getListenerKey(const char* operatorId, const bool after, char* key)
{
    sprintf(key, "%s-%d", operatorId, after);
}


bool registListener(    const InstanceData* instance, const char* method, const NPObject* npobj, const NPVariant* args, const uint32_t argCount, NPVariant* result)
{
    // assert(argCount == 3);
    if (argCount != 3)
    {
        return false;
    }

    char operatorId[NAME_SIZE] = {0};
    npvariant_to_buffer(&(args[0]), operatorId);

    bool after = (bool)NPVARIANT_TO_BOOLEAN(args[2]);

    char key[NAME_SIZE] = {0};
    getListenerKey(operatorId, after, key);

    std::map<std::string, JsListener*> jsListeners = ((NPPluginApp*)(instance->pNPPluginApp))->jsListeners;
    //目前只支持一个
    JsListener* pListener = jsListeners[key];
    if (pListener != NULL)
    {
        jsListeners[key] = NULL;
        delete pListener;
    }
    pListener = new JsListener(&args[0], &args[1], &args[2]);
    jsListeners[key] = pListener;
    
    char* id = (char*)NPN_MemAlloc(NAME_SIZE);
    sprintf(id, "%d", pListener->id);
    STRINGZ_TO_NPVARIANT(id, *result);

    //释放会出错
    //NPN_MemFree(id);
    
    return true;
}

bool unRegistListener(    const InstanceData* instance, const char* method, const NPObject* npobj, const NPVariant* args, const uint32_t argCount, NPVariant* result)
{
    if (argCount != 1)
    {
         return false;
    }
    
    char param1[NAME_SIZE] = {0};
    npvariant_to_buffer(&(args[0]), param1);
    if (strlen(param1) == 0)
    {
        return false;
    }
    
    int id = atoi(param1);
    std::map<std::string, JsListener*> jsListeners = ((NPPluginApp*)(instance->pNPPluginApp))->jsListeners;
    std::map<std::string, JsListener*>::iterator iter;
        for (iter=jsListeners.begin(); iter != jsListeners.end(); iter++)
        {
            if (id == iter->second->id && iter->second != NULL)
            {
                delete iter->second;
                iter->second = NULL;
                jsListeners.erase(iter++);
                break;
            }
        }
    return true;
}

bool onListenerAction(const InstanceData* instance, const char* operatorId, const bool after)
{
    char key[128] = {0};
    getListenerKey(operatorId, after, key);
    
    std::string key_str(key);
    std::map<std::string, JsListener*> jsListeners = ((NPPluginApp*)(instance->pNPPluginApp))->jsListeners;

    JsListener* pListener = jsListeners[key_str];
    if (pListener != NULL)
    {
        NPObject* npWindow = NULL;
        NPN_GetValue(instance->npp, NPNVWindowNPObject, &npWindow);
        
        NPVariant windowVar;
        NPN_GetProperty(instance->npp, npWindow, NPN_GetStringIdentifier("window"), &windowVar);
        
        NPObject *window = NPVARIANT_TO_OBJECT(windowVar);
        
        NPVariant resp;
        NPN_Invoke(instance->npp,
          window,
            NPN_GetStringIdentifier(pListener->callback),
            NULL, 0,
            &resp);
    }
    return true;
}

