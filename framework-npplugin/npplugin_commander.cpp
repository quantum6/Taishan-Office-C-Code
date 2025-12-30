
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <assert.h>

#ifdef XP_WIN
#include <process.h>
#include <float.h>
#include <windows.h>
#define getpid _getpid
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include "npplugin_app.h"
#include "npplugin_config.h"
#include "npplugin_js_listener.h"
#include "npplugin_js_function.h"

using namespace std;


#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))
#define STATIC_ASSERT(condition)                                \
    extern void np_static_assert(int arg[(condition) ? 1 : -1])

static int jsfunction_array_length = ARRAY_LENGTH(saPluginJsFunctions);

static NPUTF8*            sPluginJsMethodIdentifierNames[ARRAY_LENGTH(saPluginJsFunctions)];
static NPIdentifier       sPluginJsMethodIdentifiers    [ARRAY_LENGTH(saPluginJsFunctions)];



#if defined(XP_UNIX)
NP_EXPORT(const char*) NP_GetMIMEDescription()
#elif defined(XP_WIN) || defined(XP_OS2)
const char* NP_GetMIMEDescription()
#endif
{
  return NPPLUGIN_PRODUCT_MIME;
}

// {{{{{{{{{{

const char* getNpPluginName()
{
	return NPPLUGIN_PRODUCT_NAME;
}

const char* getNpPluginVersion()
{
    return NPPLUGIN_PRODUCT_BUILD_VERSION;
}

const char* getNpPluginDescription()
{
    return NPPLUGIN_PRODUCT_MIME;
}

// }}}}}}}}}}

bool
scriptableEnumerate(NPObject* npobj, NPIdentifier** identifier, uint32_t* count)
{
  const int bufsize = sizeof(NPIdentifier) * ARRAY_LENGTH(sPluginJsMethodIdentifierNames);
  NPIdentifier* ids = (NPIdentifier*) NPN_MemAlloc(bufsize);
  if (!ids)
    return false;

  memcpy(ids, sPluginJsMethodIdentifiers, bufsize);
  *identifier = ids;
  *count = ARRAY_LENGTH(sPluginJsMethodIdentifierNames);
  return true;
}


bool
scriptableHasMethod(NPObject* npobj, NPIdentifier name)
{
  for (int i = 0; i < int(ARRAY_LENGTH(sPluginJsMethodIdentifiers)); i++) {
    if (name == sPluginJsMethodIdentifiers[i])
      return true;
  }
  return false;
}


void initializeIdentifiers()
{
  if (!sIdentifiersInitialized)
  {
    for (int i=0; i<jsfunction_array_length; i++)
    {
      sPluginJsMethodIdentifierNames[i] = (NPUTF8*)saPluginJsFunctions[i];
    }

    NPN_GetStringIdentifiers((const NPUTF8**)sPluginJsMethodIdentifierNames,
       ARRAY_LENGTH(sPluginJsMethodIdentifierNames),
       sPluginJsMethodIdentifiers);
    NPN_GetStringIdentifiers(sPluginPropertyIdentifierNames,
       ARRAY_LENGTH(sPluginPropertyIdentifierNames),
       sPluginPropertyIdentifiers);

    sIdentifiersInitialized = true;    

    // Check whether NULL is handled in NPN_GetStringIdentifiers
    NPIdentifier IDList[2];
    static char const *const kIDNames[2] = { NULL, "setCookie" };
    NPN_GetStringIdentifiers(const_cast<const NPUTF8**>(kIDNames), 2, IDList);
  }
}

void clearIdentifiers()
{
  memset(sPluginJsMethodIdentifiers, 0,
      ARRAY_LENGTH(sPluginJsMethodIdentifiers)   * sizeof(NPIdentifier));
  memset(sPluginPropertyIdentifiers, 0,
      ARRAY_LENGTH(sPluginPropertyIdentifiers) * sizeof(NPIdentifier));

  sIdentifiersInitialized = false;
}


bool
scriptableInvoke(NPObject* npobj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
  NPP npp = static_cast<TestNPObject*>(npobj)->npp;
  InstanceData* id = static_cast<InstanceData*>(npp->pdata);
  if (id->throwOnNextInvoke) {
    id->throwOnNextInvoke = false;
    if (argCount == 0) {
      NPN_SetException(npobj, NULL);
    }
    else {
      for (uint32_t i = 0; i < argCount; i++) {
        const NPString* argstr = &NPVARIANT_TO_STRING(args[i]);
        NPN_SetException(npobj, argstr->UTF8Characters);
      }
    }
    return false;
  }
  
  for (int i = 0; i < int(ARRAY_LENGTH(saPluginJsFunctions)); i++)
  {
      if (name != sPluginJsMethodIdentifiers[i])
      {
          continue;
      }

      NPP mNpp = static_cast<TestNPObject*>(npobj)->npp;
      InstanceData* instance = static_cast<InstanceData*>(static_cast<TestNPObject*>(npobj)->npp->pdata);

      //OnStartApp
      //OnCloseApp
      //OnModifyChange
            
      char* funName = (char*)(saPluginJsFunctions[i]);
      if (!strcmp(funName, "init"))
      {
          return JS_FUNCTION_init(result);
      }
      if (!strcmp(funName, "getPluginVersion"))
      {
          return JS_FUNCTION_getPluginVersion(result);
      }

      bool isAfter = false;
      if (!strcmp(funName, "saveAs"))
      {
          onListenerAction(instance, "OnSaveAs",        isAfter);
          onListenerAction(instance, "OnSave",          isAfter);
      }
      else if (!strcmp(funName, "close"))
      {
          onListenerAction(instance, "OnPrepareUnload", isAfter);
      }
      else if (!strcmp(funName, "print"))
      {
          onListenerAction(instance, "OnPrint",         isAfter);
      }
          
      JsCommander* pCommander = ((NPPluginApp*)(instance->pNPPluginApp))->pCommander;
      pCommander->sendCommandFunction(funName, args, argCount);
      bool ret = pCommander->receiveResult(instance, result);
          
      isAfter = true;
      if (!strcmp(funName, "createDocument"))
      {
          onListenerAction(instance, "OnCreate",       isAfter);
          onListenerAction(instance, "OnNew",          isAfter);
          onListenerAction(instance, "OnFocus",        isAfter);
      }
      else if (!strcmp(funName, "openDocument"))
      {
          onListenerAction(instance, "OnLoadFinished", isAfter);
          onListenerAction(instance, "OnLoad",         isAfter);
          onListenerAction(instance, "OnFocus",        isAfter);
      }
      else if (!strcmp(funName, "saveAs"))
      {
          onListenerAction(instance, "OnSaveAsDone",   isAfter);
          onListenerAction(instance, "OnSaveDone",     isAfter);
      }
      else if (!strcmp(funName, "close"))
      {
          onListenerAction(instance, "OnUnload",       isAfter);
          onListenerAction(instance, "OnUnfocus",      isAfter);
      }

      return ret;
  }
  return false;
}
