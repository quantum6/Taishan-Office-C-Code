
#ifndef __NPPLUGIN_JS_LISTENER_H__
#define __NPPLUGIN_JS_LISTENER_H__

#include <map>

#include "npplugin_sdk.h"
#include "npplugin_js_function.h"


#define NAME_SIZE 128

/**
create listener id.
 */
static int listener_counter = 0;

void npvariant_to_buffer(const NPVariant* var, char* buffer);

class JsListener
{
public:
        JsListener(const NPVariant* oid, const NPVariant* cb, const NPVariant* af)
        {
            memset(operateId, 0, NAME_SIZE);
            npvariant_to_buffer(oid, operateId);
            
            memset(callback,  0, NAME_SIZE);
            npvariant_to_buffer(cb, callback);
            
            this->after = (bool)NPVARIANT_TO_BOOLEAN(*af);
            id = listener_counter;
            listener_counter ++;
        }

public:
    char  operateId[NAME_SIZE];
    char  callback [NAME_SIZE];
    bool  after;
    int   id;

};

bool registListener(  const InstanceData* instance, const char* method, const NPObject* npobj, const NPVariant* args, const uint32_t argCount, NPVariant* result);
bool unRegistListener(const InstanceData* instance, const char* method, const NPObject* npobj, const NPVariant* args, const uint32_t argCount, NPVariant* result);
bool onListenerAction(const InstanceData* instance, const char* operatorId, const bool after);

#endif // __NPPLUGIN_JS_LISTENER_H__
